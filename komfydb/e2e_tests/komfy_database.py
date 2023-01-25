from pathlib import Path
from subprocess import Popen, PIPE
from utils import timeit
import fcntl
import os
import time

class KomfyDatabase():
    def __init__(self):
        self.db_process = Popen(["bazel", "run", "//komfydb:e2e_tests"], stdin=PIPE, stdout=PIPE, text=True)
        self.db_process.stdin.reconfigure(line_buffering=True)
        fd = self.db_process.stdout.fileno()
        fl = fcntl.fcntl(fd, fcntl.F_GETFL)
        fcntl.fcntl(fd, fcntl.F_SETFL, fl | os.O_NONBLOCK)

    def readline(self) -> str:
        return self.db_process.stdout.readline()

    def write(self, data: str) -> int:
        return self.db_process.stdin.write(data)

    def assert_done(self):
        EXPECTED_MESSAGE = "DONE\n"
        response = self.readline()
        assert(response == EXPECTED_MESSAGE)

    def load(self, db_file: Path) -> None:
        """run a binary KomfyDb as subprocess with connected stdin/out"""

        # create database
        with db_file.open() as file:
            for query in file:
                self.write(query)
        
        # get rid of all the output
        time.sleep(1)
        while True:
            trash = self.readline()
            if trash:
                print(trash)
                continue
            break

        # self.assert_done()

        # STATS_QUERY = "ANALYZE;\n"
        # self.write(STATS_QUERY)

        # self.assert_done()

    @timeit
    def execute(self, query: str):
        self.write(query)
        response = self.readline()
        if response:
            try:
                python_obj_response = eval(response)
            except SyntaxError:
                raise ValueError(f"invalid response from komfydb: {response}")

            return python_obj_response



db = KomfyDatabase()
db.load(Path("database.sql"))
