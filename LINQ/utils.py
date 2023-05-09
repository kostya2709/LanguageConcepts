import re

def Fibonacci():
    a, b = 0, 1
    while True:
        yield a
        a, b = b, a + b

def read_lines(filename):
    with open(filename, "r+") as file:
        while True:
            line = file.readline()
            if line == "":
                break
            yield line

def split(str):
    delimiters = r"[\n\t,.\-!? ]+"
    return [x for x in re.split(delimiters, str) if x != ""]
