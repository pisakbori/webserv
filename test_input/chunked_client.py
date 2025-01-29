import requests


def chunked_lorem_ipsum():
    lorem_text = (
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. ",
        "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ",
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat."
    )
    for chunk in lorem_text:
        yield chunk.encode("utf-8")  

url = "http://localhost:8080/post-endpoint" 

headers = {
    "Transfer-Encoding": "chunked",
    "Content-Type": "application/json",
    "Host": "localhost"
}

response = requests.post(url, data=chunked_lorem_ipsum(), headers=headers, stream=True)

print("Response status:", response.status_code)
print("Response body:", response.text)