from pathlib import Path
import sqlite3

from database import Database
from utils import timeit

class SQLiteDatabase(Database):
    def __init__(self):
        self.connection = sqlite3.connect(':memory:')
        self.cursor = self.connection.cursor()

    def __repr__(self) -> str:
        return "SQLiteDB"

    def load(self, db_file: Path):
        script = db_file.read_text()
        self.cursor.executescript(script)

    @timeit
    def execute(self, query: str):
        return list(self.cursor.execute(query))

    # def execute_multiple(self, sql_script: str)
        