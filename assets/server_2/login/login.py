import sys
import os

res = "HTTP/1.1 302 Found\r\n"
res += "Content-Type: text/html\r\n"
if len(sys.argv) == 4:
    res += f"Set-Cookie: username={sys.argv[1]}\n"
    res += f"Set-Cookie: password={sys.argv[2]}\n"

res += "Location: /login/logged_in.html\r\n"
res += "Content-Length: 0\r\n\r\n"
print(res)