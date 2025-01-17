# location - Configuration Directive

## Syntax

```
location [ = ] uri { ... }
```

## Description

The `location` directive is used to set configuration blocks depending on a request URI. The matching is performed against a normalized URI, which involves:

- Decoding percent-encoded characters in the `"%XX"` form.
- Resolving references to relative path components `"."` and `".."`.
- Compressing two or more adjacent slashes into a single slash.

A location is defined by a prefix string, and webserv performs the matching based on the longest prefix. When multiple location blocks match a URI, NGINX selects the one with the longest prefix.

## Modifiers

- **`=`**: Exact match. If an exact match is found, the search terminates immediately, improving performance when the request URI is known to match exactly.
- **`@name`**: Named location. These locations are not used for regular request processing, but are useful for redirects or other internal processing.

## Example

### Basic Matching

```nginx
location = / {
    # Configuration A
}

location / {
    # Configuration B
}

location /documents/ {
    # Configuration C
}
```

- The request for `/` will match **Configuration A**.
- The request for `/index.html` will match **Configuration B**.
- The request for `/documents/document.html` will match **Configuration C**.

### Prefix Matching

If a location is defined by a prefix string that ends with the slash character, and requests are processed by one of the following directives:

- `proxy_pass`
- `fastcgi_pass`
- `uwsgi_pass`
- `scgi_pass`
- `memcached_pass`
- `grpc_pass`

A special processing occurs when the URI has the same string but without the trailing slash. In such a case, NGINX will return a permanent redirect with a 301 status code, appending the trailing slash to the URI. To avoid this behavior, you can define an exact match for the URI and location, like this:

```nginx
location /user/ {
    proxy_pass http://user.example.com;
}

location = /user {
    proxy_pass http://login.example.com;
}
```

In this case, the location `/user/` will be processed by `proxy_pass http://user.example.com`, while the exact match `/user` will be processed by `proxy_pass http://login.example.com`.

## Function: Finding Matching Location

Given a server and a request URI, this function determines which location matches the URI and returns the complete matched path. If no exact match is found, a default value (such as `/index.html`) will be used, based on the configuration.

### Parameters

- `server`: The virtual server.
- `request_uri`: The URI of the incoming request.

### Return Value

- `location`: The location configuration that matches the request URI.

### Example

# If the request URI is `/index.html`, the function will find the longest matching location and return the appropriate configuration, such as `location / { ... }` or `location = /index.html { ... }`.

one name per server. Server config is uniquely identified by port, host, name

TODO:Bori

# execve a program and read from the programs std output

The read must go through select
If someone requests mock.py
then fork execve python3 mock.py
And read from the std out

<!-- how to use: -->

./webserv
nc -w 5 localhost 8081 < ./test_input/bla.txt

<!-- or line by line: -->

nc localhost 8080
GET /index.html?user=123 HTTP/1.1
ju:hi

end GET request with empty line

TODO:Marian
make sure parsing trims white spaces correctly
