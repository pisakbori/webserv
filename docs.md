Invalid Characters in HTTP Requests
--> Bad Request (400)

Control Characters (ASCII 0-31 and 127):
Examples: \x00, \x1F, DEL (\x7F)
These are not allowed in HTTP requests, except for the proper use of CR (\r, \x0D) and LF (\n, \x0A) as line terminators.
Unescaped Special Characters in URLs:

Characters like space ( ), #, <, >, {, }, |, \, ^, ~, [, ], and backtick (```) cannot appear unescaped in a URL.
They must be percent-encoded (e.g., a space becomes %20).

Invalid or Reserved Characters in Headers:
Header names can only include ASCII alphanumeric characters and - or \_.
Header values cannot contain unescaped control characters or line breaks.

Non-ASCII Characters:
HTTP 1.1 does not natively support non-ASCII characters in headers or URLs. These should be encoded using percent-encoding for URLs or proper character encoding like UTF-8 for headers.
