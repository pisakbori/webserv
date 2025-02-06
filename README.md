# Webserv - A HTTP/1.1 Server

The goal of this project was to implement a custom web server from scratch in C++ to deepen our understanding of the HTTP protocol, socket programming, and I/O multiplexing. We aimed to recreate core functionalities similar to NGINX.

Webserv is a non-blocking HTTP server that supports GET, POST, and DELETE methods while serving static content. By using select() for I/O operations, the server is capable of handling multiple concurrent connections efficiently.

In addition to static file serving, Webserv supports file uploads and can execute CGI scripts, providing flexibility in handling dynamic content. The server's behavior is configurable through a setup file, allowing it to listen on multiple ports, define routes, and respond to specific HTTP methods.

