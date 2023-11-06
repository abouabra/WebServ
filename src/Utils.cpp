#include "../includes/Utils.hpp"

void guard(int status, std::string message)
{
	message = message + "\n" + strerror(errno);
	if(status < 0)
	{
		log(message, ERROR);
		throw std::exception();
	}
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
	std::fstream file(name.c_str(), std::ios::in);
	std::string content;
	std::string line;


	while(std::getline(file, line))
		content += line + "\n";
	return content;
}
