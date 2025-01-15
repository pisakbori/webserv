#!/usr/bin/env python3
# import time
import sys

# print("Content-Type: text/html")
# print()  

# print("<html>")
# time.sleep(2) 
# print("<body>")
# print("<h1>Hello, World!</h1>")
# print("</body>")
# print("</html>")


def generate_large_output(size_in_mb):
    chunk = "A" * 1024  # 1 KB chunk of data
    chunks_per_mb = 1024  # 1 MB of data
    total_chunks = size_in_mb * chunks_per_mb

    for _ in range(total_chunks):
        sys.stdout.write(chunk)
        sys.stdout.flush()  # Ensure data is written immediately

generate_large_output(10)
