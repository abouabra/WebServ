#include "../includes/Config.hpp"
#include "../includes/Utils.hpp"
#include "../includes/Server.hpp"

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
		// config.print_config();

		Server_Config server_1;
		Routes route_1;
		route_1.set_path("/");
		route_1.set_default_file("index.html");
		std::vector<std::string> vs;
		vs.push_back("GET");
		vs.push_back("POST");
		route_1.set_methods(vs);
		std::vector<Routes> v;
		v.push_back(route_1);
		server_1.set_routes(v);
		server_1.set_port(2003);
		server_1.set_client_body_limit(1024 * 1024);
		config.servers.push_back(server_1);

		Server_Config server_2;
		server_2.set_port(2004);
		server_2.set_client_body_limit(1024 * 1024);
		config.servers.push_back(server_2);


		Server_Config server_3;
		server_3.set_port(2005);
		server_3.set_client_body_limit(1024 * 1024);
		config.servers.push_back(server_3);

		Server server(config);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
