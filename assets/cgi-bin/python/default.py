#!/usr/bin/env python3
import sys

if len(sys.argv) != 4:
	print("Usage: python3 default.py <first_name> <last_name> <message>")
	sys.exit(1)

# HTML header
html_header = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Python CGI</title>
    <style>
        body{
            background-color: #000;
        }
        .container{
            width: 100%;
            height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            flex-direction: column; /* Added to stack the headings vertically */

        }
		.items{
			display: flex;
			flex-direction: column;
			gap: 10px;
			margin: 3%;
		}
        h1{
            color: #ffd43b;
            font-size: 50px;
            margin: 10px 0;
			margin: 3%;

        }
		h2{
            color: #ffd43b;
            font-size: 50px;
            margin: 10px 0;
        }
    </style>
</head>
<body>
	<div class="container">
		<h1>Python CGI</h1>
        <div class="items">
"""

# HTML footer
html_footer = """
		</div>
    </div>
</body>
</html>
"""

# Print the formatted arguments
print(html_header)
print(f"<h2>First Name: {sys.argv[1]}</h2>")
print(f"<h2>Last Name: {sys.argv[2]}</h2>")
print(f"<h2>Message: {sys.argv[3]}</h2>")
print(html_footer)

while True:
    pass