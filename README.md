one name per server. Server config is uniquely identified by port, host, name

TODO:Bori

- parse at each write
- head ends at \r\n\r\n; don't consider body for now.
- after head is read/on error, send "meow" or error back
- if request hasn't finished before timeout, error
