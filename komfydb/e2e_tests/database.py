from abc import ABC, abstractmethod
from pathlib import Path


class Database(ABC):
    @abstractmethod
    def load(self, db_file: Path) -> None:
        """load database from sql file"""
        ...

    @abstractmethod
    def execute(self, query: str):
        """execute a single query and return results"""
        ...


