#!/usr/bin/env python3

from scripts import make_kernel
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--debug", action=argparse.BooleanOptionalAction, default=True)
    args = parser.parse_args()
    make_kernel(**vars(args))
