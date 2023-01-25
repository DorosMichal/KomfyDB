import argparse
from pathlib import Path
import sys
import logging
from sqlite_database import SQLiteDatabase
from komfy_database import KomfyDatabase
from test_runner import TestRunner

DATABASE_SCRIPT = Path("database.sql")


def main():
    logging.basicConfig(encoding='utf-8', level=logging.INFO)
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input_queries", type=argparse.FileType('r'), default=sys.stdin)
    parser.add_argument("-v", "--verbose", action='store_true')
    args = parser.parse_args()
    
    db1 = SQLiteDatabase()
    db2 = KomfyDatabase()

    test_runner = TestRunner(db1, db2, args.verbose)
    test_runner.prepare(DATABASE_SCRIPT)
    test_runner.run_tests(args.input_queries)
    

if __name__ == "__main__":
    main()