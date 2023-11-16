#!/bin/bash

# Check if exactly three arguments are provided
if [ $# -ne 3 ]; then
	echo "Usage: bash default.sh <first_name> <last_name> <message>"
    exit 1
fi

# HTML header
html_header=$(cat <<EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>BASH CGI</title>
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
            color: #fff;
            font-size: 50px;
            margin: 10px 0;
			margin: 3%;

        }
		h2{
            color: #fff;
            font-size: 50px;
            margin: 10px 0;
        }
    </style>
</head>
<body>
	<div class="container">
		<h1>BASH CGI</h1>
        <div class="items">
EOF
)

# HTML footer
html_footer=$(cat <<EOF
		</div>
    </div>
</body>
</html>

EOF
)

# Print the formatted arguments with HTML structure
echo "$html_header"
echo "<h2>First Name: $1</h2>"
echo "<h2>Last Name: $2</h2>"
echo "<h2>Message: $3</h2>"
echo "$html_footer"
