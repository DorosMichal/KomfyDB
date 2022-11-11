import random
import sys

f_name = sys.argv[1]
n_tuples = 4096 * 8 // (272 * 8 + 4)

with open(f_name, "w") as f:
    for i in range(1,n_tuples+1):
        f.write(f"{i},{'a' * (i)},{i},{'b'*(i)}\n")
