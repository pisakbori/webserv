import os

# Print the HTTP headers
print("Content-Type: text/html\n")

# Start the HTML document
print("<html>")
print("<head><title>CGI Environment Variables</title></head>")
print("<body>")
print("<h1>CGI Environment Variables</h1>")

# Display each environment variable in a table
print("<table border=\"1\">")
print("<tr><th>Variable</th><th>Value</th></tr>")

# Output some common CGI environment variables
print("<tr><td>QUERY_STRING</td><td>{}</td></tr>".format(os.environ.get("QUERY_STRING", "")))
print("<tr><td>REQUEST_METHOD</td><td>{}</td></tr>".format(os.environ.get("REQUEST_METHOD", "")))
print("<tr><td>SCRIPT_NAME</td><td>{}</td></tr>".format(os.environ.get("SCRIPT_NAME", "")))
print("<tr><td>PATH_INFO</td><td>{}</td></tr>".format(os.environ.get("PATH_INFO", "")))
print("<tr><td>SERVER_SOFTWARE</td><td>{}</td></tr>".format(os.environ.get("SERVER_SOFTWARE", "")))
print("<tr><td>REMOTE_ADDR</td><td>{}</td></tr>".format(os.environ.get("REMOTE_ADDR", "")))
print("<tr><td>CWD</td><td>{}</td></tr>".format(os.getcwd(), ""))

# End the table and HTML document
print("</table>")
print("</body>")
print("</html>")