#!/usr/bin/env python3
import time

print("Content-Type: text/html")
print("CONTENT-LENGTH: 800")
print("Status: 404 Blabla")
print()  

print("<html>")
print("<body>")
print("<h1>Hello, World!</h1>")
print("</body>")
print("</html>")

# Sleep for 1 second
time.sleep(1)

raise Exception("Sorry, I have to fail here")