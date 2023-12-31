import ctypes

libc = ctypes.cdll.LoadLibrary("./fib.so")


def main():
    print(libc.fib(40))


if __name__ == "__main__":
    main()
