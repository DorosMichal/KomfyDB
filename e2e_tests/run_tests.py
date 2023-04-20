import argparse
from pathlib import Path
import sys
import logging
from sqlite_database import SQLiteDatabase
from komfy_database import KomfyDatabase
from test_runner import TestRunner

DATABASE_SCRIPT = Path("e2e_tests/database.sql")


def main():
    logging.basicConfig(encoding='utf-8', level=logging.INFO)
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input_queries", type=argparse.FileType('r'), default=sys.stdin)
    parser.add_argument("-v", "--verbose", action='store_true')
    args = parser.parse_args()

    db1 = SQLiteDatabase()
    db2 = KomfyDatabase()

    try:
        test_runner = TestRunner(db1, db2, args.verbose)
        test_runner.prepare(DATABASE_SCRIPT)
        test_runner.run_tests([
            'SELECT * FROM table1;\n',
            'SELECT * FROM table1 t1, table1 t2 WHERE t1.a > t2.a\n',
            'SELECT * FROM table1 t1, table1 t2, table1 t3 WHERE t1.pk > t2.pk AND t2.pk > t3.pk\n'
        ])
    except Exception:
        exit(1)


if __name__ == "__main__":
    main()
