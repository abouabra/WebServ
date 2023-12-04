#include "../includes/Utils.hpp"

void guard(int status, std::string message)
{
	message = message + "\n" + std::strerror(errno);
	if(status < 0)
	{
		// log(message, ERROR);
		throw std::runtime_error(message);
	}
}


std::string addr_to_ip(in_addr_t number)
{
    std::string ip;
    unsigned char bytes[4];

    number = ntohl(number);

    bytes[0] = (number >> 24) & 255;
    bytes[1] = (number >> 16) & 255;
    bytes[2] = (number >> 8) & 255;
    bytes[3] = number & 255;

    for (int i = 0; i < 4; ++i)
    {
        std::stringstream ss;
        ss << static_cast<unsigned int>(bytes[i]); // Convert byte to string
        ip += ss.str();
        if (i != 3)
            ip += '.'; // Add dot after each byte except the last one
    }

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
		delete arr[i];
	delete [] arr;
}

std::string ft_trim(std::string str, std::string charset)
{
	std::string::size_type pos = str.find_last_not_of(charset);
	if(pos != std::string::npos)
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of(charset);
		if(pos != std::string::npos)
			str.erase(0, pos);
	}
	else
		str.erase(str.begin(), str.end());
	return str;
}

int is_valid_ip(std::string ip)
{
	std::stringstream ss(ip);
	std::string token;
	int count = 0;
	while(std::getline(ss, token, '.'))
	{
		if(token.length() > 3 || token.length() == 0)
			return 0;
		if(token.length() > 1 && token[0] == '0')
			return 0;
		for(size_t i = 0; i < token.length(); i++)
		{
			if(!isdigit(token[i]))
				return 0;
		}
		std::stringstream tmp(token);
		int num;
		tmp >> num;
		if(num < 0 || num > 255)
			return 0;
		count++;
	}
	if(count != 4)
		return 0;
	return 1;
}

int ft_atoi(std::string str)
{
	int num = 0;
	int sign = 1;
	if(str[0] == '-')
	{
		sign = -1;
		str.erase(0, 1);
	}
	for(size_t i = 0; i < str.length(); i++)
	{
		if(!isdigit(str[i]))
			return -1;
		num = num * 10 + (str[i] - '0');
	}
	return num * sign;
}

void check_multiple(std::string &value, std::stringstream &ss_2)
{
	int count = 1;
	while (ss_2 >> value)
		count++;
	if(count != 1)
		throw(std::runtime_error("Invalid config number of arguments"));
}
