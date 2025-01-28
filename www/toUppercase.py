#!/usr/bin/env python3

import sys


def main():
    # Read input from stdin
    input_data = sys.stdin.read()
    
    # Convert to uppercase
    uppercase_data = input_data.upper()
    
    # Output the uppercase result
    print(uppercase_data, end="")  # Use end="" to avoid an extra newline

if __name__ == "__main__":
    main()