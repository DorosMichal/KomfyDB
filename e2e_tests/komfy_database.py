from pathlib import Path
from time import sleep
from signal import (
    SIG_IGN,
    SIGUSR1 as LOADING_DONE,
    SIGUSR2 as QUERY_DONE,
    sigwait,
    signal,
    pthread_sigmask,
    SIG_BLOCK,
)
from subprocess import Popen, PIPE
import time
from typing import List, Optional, Union
from database import Database, DatabaseRow
from utils import timeit
import fcntl
import os
from ast import literal_eval
from logging import info, error


QUERY_DONE_FLAG = False


def query_done_handler(signum, frame):
    global QUERY_DONE_FLAG
    QUERY_DONE_FLAG = True


signal(QUERY_DONE, query_done_handler)


class KomfyDatabase(Database):
    def __init__(self):
        """signal(LOADING_DONE, SIG_IGN)"""
        pthread_sigmask(SIG_BLOCK, {LOADING_DONE})
        self.db_process = Popen(
            ["./komfydb/e2e_tests", "--logtostderr=1"],
            stdin=PIPE,
            stdout=PIPE,
            text=True,
        )
        self.db_process.stdin.reconfigure(line_buffering=True)
        fd = self.db_process.stdout.fileno()
        fl = fcntl.fcntl(fd, fcntl.F_GETFL)
        fcntl.fcntl(fd, fcntl.F_SETFL, fl | os.O_NONBLOCK)

    def __repr__(self) -> str:
        return "KomfyDB"

    def readline(self) -> str:
        return self.db_process.stdout.read(4096)

    def write(self, data: str) -> int:
        return self.db_process.stdin.write(data)

    def send_sync(self) -> None:
        self.write("SYNC;\n")

    def load(self, db_file: Path) -> None:
        """run a binary KomfyDb as subprocess with connected stdin/out"""

        # check subprocess status
        if self.db_process.poll() is not None:
            assert False

        # create database
        with db_file.open() as file:
            for query in file:
                self.write(query)

        self.send_sync()

        # get rid of all the output
        sigwait({LOADING_DONE})

        while True:
            trash = self.readline()
            if trash:
                continue
            break

        if self.db_process.poll() is not None:
            assert False
        # STATS_QUERY = "ANALYZE;\n"
        # self.write(STATS_QUERY)

    @timeit
    def execute(
        self, query: str, ignore_response: bool = False
    ) -> Optional[List[DatabaseRow]]:
        global QUERY_DONE_FLAG
        QUERY_DONE_FLAG = False
        self.write(query)
        responses = []
        response = None
        while (
            (response := self.readline())
            or not QUERY_DONE_FLAG
            or (response := self.readline())
        ):
            if response:
                responses.append(response)

        if ignore_response:
            return

        full_response = "".join(responses)

        try:
            python_obj_response = literal_eval(full_response)
            return python_obj_response

        except SyntaxError:
            error(
                f"invalid response from komfydb, {QUERY_DONE_FLAG = }, \
                    {responses = }"
            )
            raise
