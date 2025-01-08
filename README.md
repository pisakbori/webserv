one name per server. Server config is uniquely identified by port, host, name

TODO:Bori

- how to match request uri with location?
- check if request method accepted
- always serve 1 file back instead of meow.

<!-- how to use: -->

./webserv
cat ./test_input/GET.txt | nc localhost 8080

<!-- or line by line: -->

nc localhost 8080
GET /index.html?user=123 HTTP/1.1
ju:hi

end GET request with empty line
