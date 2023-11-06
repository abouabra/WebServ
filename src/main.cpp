#include "../includes/config.hpp"

int main(int ac, char **av)
{
	std::string config_file;

	if(ac > 2)
	{
		std::cout << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}
	if(ac == 2)
		config_file = av[1];
	else
		config_file = "config/default.conf";
	try
	{
		Config config(config_file);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}