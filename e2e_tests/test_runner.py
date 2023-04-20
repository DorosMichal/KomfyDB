from pathlib import Path
from typing import List
from database import Database
from logging import info, error


class TestRunner:
    def __init__(self, db1: Database, db2: Database, verbose=False):
        self.db1 = db1
        self.db2 = db2
        self.verbose = verbose

    def prepare(self, db_file: Path):
        self.db1.load(db_file)
        self.db2.load(db_file)

    def run_test(self, query):
        info(f"Running test for query: {query}")
        result1, time1 = self.db1.execute(query)
        result2, time2 = self.db2.execute(query)

        if result1 == result2:
            info(f"query {query} finished successfully; {time1 = }, {time2 = }\n")
        else:
            error(f"different results on query {query}")
            if self.verbose:
                error(f"{result1 = }\n{result2 = }")
            assert(False)

    def run_tests(self, queries: List[str]):
        info(f"1. {self.db1}, 2. {self.db2}")
        for query in queries:
            self.run_test(query)

