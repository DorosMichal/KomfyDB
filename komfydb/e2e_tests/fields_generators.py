
from enum import Enum
import random
from string import ascii_letters
from typing import List, TypeVar
from uuid import uuid4

class ValueType(Enum):
    INT = "INT"
    TEXT = "TEXT"

class GenerationType(Enum):
    RANGE = 0
    ENUM = 1
    UNIQUE = 2
    FOREIGN = 3

def generate_n_letter_str(n: int) -> str:
    return "".join(random.choices(ascii_letters, k=n))


def generate_random_str(max_len: int) -> str:
    length = random.randint(1, max_len)
    return generate_n_letter_str(length)


def generate_range_int(size: int, start: int, stop: int, number=None) -> List[int]:
    choices = range(start, stop)
    if number is not None:
        choices = random.choices(range(start, stop), k=number)
    
    return random.choices(choices, k=size)


def generate_range_str(size:int , start: int, stop: int, number=None) -> List[str]:
    if number is not None:
        choices = [generate_n_letter_str(n) for n in random.choices(range(start, stop), k=number)]
        return random.choices(choices, k=size)
    
    return [generate_n_letter_str(n) for n in random.choices(range(start, stop), k=size)]


def generate_unique_int(size: int) -> List[int]:
    INT_MAX = 2147483647
    return random.sample(range(INT_MAX + 1), k=size)


def generate_unique_str(size: int) -> List[str]:
    result = set()
    while len(result) != size:
        result.add(str(uuid4()))
    return list(result)

T = TypeVar('T')

def generate_from_choices(size: int, choices: List[T]) -> List[T]:
    return random.choices(choices, k=size)

def make_quoted_str(func):
    def decorated_func(*args, **kwargs):
        result = func(*args, **kwargs)
        return [f"'{s}'" for s in result]
    return decorated_func

type_map = {
    ValueType.INT: {
        GenerationType.RANGE :   generate_range_int,
        GenerationType.ENUM :    generate_from_choices,
        GenerationType.UNIQUE :  generate_unique_int,
        GenerationType.FOREIGN : generate_from_choices,
    },
    ValueType.TEXT: {
        GenerationType.RANGE :   make_quoted_str(generate_range_str),
        GenerationType.ENUM :    make_quoted_str(generate_from_choices),
        GenerationType.UNIQUE :  make_quoted_str(generate_unique_str),
        GenerationType.FOREIGN : generate_from_choices,
    }
}