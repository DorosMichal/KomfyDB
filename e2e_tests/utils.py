import time

def timeit(f):
    def timed(*args, **kw):
        t_start = time.time()
        result = f(*args, **kw)
        t_end = time.time()

        return result, t_end - t_start

    return timed