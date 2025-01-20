import socket
import time

# Connect to the server
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 1)
client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1)
client_socket.connect(('127.0.0.1', 8080))
print("connection made.")

# time.sleep(5)  # Delay before starting to send
print("start to send.")
# Send data slowly
extra = "a" * 20
data = "POST /html/meow.html HTTP/1.1\r\nHost: localhost\r\nContent-Length:"
data += str(len(extra))+"\r\n\r\n"
data += extra
# data += "b"

# chunk_size = 100000
chunk_size = 1000
for i in range(0, len(data), chunk_size):
    chunk = data[i:i+chunk_size]
    client_socket.send(chunk.encode())
    print("send "+ str(i))
    time.sleep(0.1)  # Delay between each byte
print("sent all")

# for char in data:
#     client_socket.send(char.encode())

# Receive and print the response
# Receive and print the response one character at a time
while True:
    response = client_socket.recv(1)  # Receive one byte (character)
    # time.sleep(0.1)  # Delay between each byte
    if not response:
        break  # No more data, exit the loop
    print(response.decode(), end="")

client_socket.close()
