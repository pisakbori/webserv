one name per server. Server config is uniquely identified by port, host, name

TODO:Bori

- how to match request uri with location?
- always serve 1 file back instead of meow.
- first attempt: just read file with fstream, ignore file descriptors

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
