from abc import ABC, abstractmethod
from pathlib import Path
from typing import List, NewType, Optional, Tuple, Union

DatabaseRow = NewType("DatabaseRow", Tuple[Union[int, str]])


class Database(ABC):
    @abstractmethod
    def load(self, db_file: Path) -> None:
        """load database from sql file"""
        ...

    @abstractmethod
    def execute(self, query: str) -> Optional[List[DatabaseRow]]:
        """execute a single query and return results"""
        ...
