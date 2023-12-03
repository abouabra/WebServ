#include "../includes/Utils.hpp"
#include <algorithm>
#include <sstream>

void guard(int status, std::string message)
{
	message = message + "\n" + strerror(errno);
	if(status < 0)
	{
		log(message, ERROR);
		throw std::exception();
	}
}

std::string  addr_to_ip(in_addr_t number)
{
	std::string ip;
    std::stringstream ss;
    std::string nm;
    number = ntohl(number);
    ss << ((number >> 24) & 255);
    ss >> nm;
    ss.clear();
    ip += nm + '.';
    ss << ((number >> 16) & 255);
    ss >> nm;
    ss.clear();
    ip += nm + '.';
    ss << ((number >> 8) & 255);
    ss >> nm;
    ss.clear();
    ip += nm + '.';
    ss << (number & 255);
    ss >> nm;
    ss.clear();
    ip += nm;
	return ip;
}

void log(std::string message, int level)
{
	time_t current;
	char buffer[100];

	current = time(NULL);
	std::memset(buffer, 0, sizeof(buffer));
	strftime(buffer, 100, "[%Y-%m-%d %H:%M:%S]", localtime(&current));


	std::map<int, std::string> log_colors;

	log_colors[INFO] = "\033[1;32m";
	log_colors[WARNING] = "\033[1;33m";
	log_colors[ERROR] = "\033[1;31m";
	log_colors[RESET] = "\033[0m";

	std::cout << log_colors[level] <<  buffer << " " << message << log_colors[RESET] <<std::endl;
}

std::string itoa(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

std::string read_file(std::string name)
{
	// name = "/Users/abouabra/1337/Cursus/WebServ/assets/" + name;
	std::fstream file(name.c_str(), std::ios::in);
	std::string content;
	std::string line;


	while(std::getline(file, line))
		content += line + "\n";
	return content;
}

std::string urlDecode(std::string str)
{
	std::string decodedString;

	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] == '%') {
			if (i + 2 < str.length()) {
				char hexDigit1 = str[i + 1];
				char hexDigit2 = str[i + 2];

				// Convert hexadecimal digits to their corresponding decimal values
				int decimalValue1 = 0;
				if (isdigit(hexDigit1))
					decimalValue1 = hexDigit1 - '0';
				else
					decimalValue1 = tolower(hexDigit1) - 'a' + 10;


				int decimalValue2 = 0;
				if (isdigit(hexDigit2)) 
					decimalValue2 = hexDigit2 - '0';
				else 
					decimalValue2 = tolower(hexDigit2) - 'a' + 10;

				// Combine the decimal values to form the decoded byte
				char decodedByte = (decimalValue1 << 4) | decimalValue2;

				decodedString += decodedByte;
				i += 2; // Skip the '%' and the two hexadecimal digits
			}
		}
		else if (str[i] == '+') // Convert the space character back to space
			decodedString += ' ';
		else
			decodedString += str[i]; // Add the non-encoded character to the decoded string
	}

	return decodedString;
}

int count_num(std::string str, char c)
{
	int count = 0;
	for(size_t i = 0; i < str.length(); i++)
	{
		if(str[i] == c)
			count++;
	}
	return count;
}

char **make_argv(std::string str, std::string cgi_bin_path, std::string cgi_bin)
{
	std::stringstream ss(str);
	std::string token;
	int count = count_num(str, '\n');
	// std::cout << "count: " << count + 3 << std::endl;
	char **argv = new char*[count + 3];
	argv[0] = strdup(cgi_bin_path.c_str());
	argv[1] = strdup(cgi_bin.c_str());
	int i = 2;
	while(std::getline(ss, token))
	{
		std::string value = token.substr(token.find('=') + 1);
		argv[i] = strdup(value.c_str());
		i++;
	}
	argv[i] = NULL;
	return argv;
}

void free_arr(char **arr)
{
	for(int i = 0; arr[i]; i++)
		free(arr[i]);
	delete [] arr;
}
