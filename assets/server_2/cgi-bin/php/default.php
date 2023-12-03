#!/usr/bin/php
<?php

// Check if exactly three arguments are provided
if ($argc != 4) {
	echo "Usage: php default.php <first_name> <last_name> <message>\n";
    exit(1);
}

// HTML structure
$html = <<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>PHP CGI</title>
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
            color: #474a8a;
            font-size: 50px;
            margin: 10px 0;
			margin: 3%;

        }
		h2{
            color: #474a8a;
            font-size: 50px;
            margin: 10px 0;
        }
    </style>
</head>
<body>
	<div class="container">
		<h1>PHP CGI</h1>
        <div class="items">
HTML;

// Formatted output with provided arguments
$html .= "<h2>First Name: " . htmlspecialchars($argv[1]) . "</h2>\n";
$html .= "<h2>Last Name: " . htmlspecialchars($argv[2]) . "</h2>\n";
$html .= "<h2>Message: " . htmlspecialchars($argv[3]) . "</h2>\n";

// HTML footer
$html .= <<<HTML
		</div>
    </div>
</body>
</html>

HTML;

// Output the HTML content
$res = "HTTP/1.1 200 OK\r\n";
$res .= "Content-Type: text/html\r\n";
$res .= "Content-Length: " . strlen($html) . "\r\n\r\n";
$res .= $html;
echo $res;
