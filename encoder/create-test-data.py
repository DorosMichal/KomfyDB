import random
import sys

f_name = sys.argv[1]
n_tuples = int(sys.argv[2])

alphabet = 'abcdefghjijklmnopqrstuvwxyz0123456789'


def rand_str():
    return ''.join(random.choices(alphabet, k=random.randint(1, 128)))


with open(f_name, "w") as f:
    for i in range(n_tuples):
        f.write(f"{random.randint(0,2**31-1)},{rand_str()},{random.randint(0,2**31-1)},{rand_str()}\n")
