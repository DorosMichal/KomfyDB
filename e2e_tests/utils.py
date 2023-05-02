import time
from typing import Callable, Tuple, TypeVar

T_return = TypeVar("T_return")


def timeit(f: Callable[..., T_return]) -> Tuple[T_return, int]:
    def timed(*args, **kw):
        t_start = time.time()
        result = f(*args, **kw)
        t_end = time.time()

        return result, t_end - t_start

    return timed
