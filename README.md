one name per server. Server config is uniquely identified by port, host, name

TODO:Bori

- how to match request uri with location?
- autoindex
- return default error page on 404

<!-- how to use: -->

./webserv
cat ./test_input/GET.txt | nc localhost 8080

<!-- or line by line: -->

nc localhost 8080
GET /index.html?user=123 HTTP/1.1
ju:hi

end GET request with empty line

TODO:Marian
make sure parsing trims white spaces correctly
