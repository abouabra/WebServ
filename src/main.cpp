#include "../includes/config.hpp"

int main(int ac, char **av)
{
	std::string file_name;

	if(ac > 2)
	{
		std::cout << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}
	if(ac == 2)
		file_name = av[1];
	else
		file_name = "config/default.conf";
	try
	{
		Config config(file_name);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
