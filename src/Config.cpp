#include "../includes/Config.hpp"
#include <cctype>
#include <exception>
#include <sstream>
#include <string>
#include <sys/_types/_size_t.h>
/*

	CONFIG CLASS

*/
Config::Config()
{

}

Config::~Config()
{

}

Config::Config(Config const &src)
{
	*this = src;
}

Config& Config::Config::operator=(Config const &obj)
{
	if (this != &obj)
		servers = obj.servers;
	return *this;
}

Config::Config(std::string file_name)
{
	log("Config file: " + file_name, INFO);
	config_file = read_config(file_name);

	// std::cout << config_file ;
	
	parse_config(config_file);
}


std::string Config::read_config(std::string &config_file)
{
	std::fstream file(config_file.c_str(), std::ios::in);
	if(!file.is_open())
	{
		log("Error: Could not open config file", ERROR);
		throw std::exception();
	}
	std::string content;
	std::string line;

	while(std::getline(file, line))
		content += line + "\n";
	return content;
}

size_t is_number(std::string &str)
{
	std::stringstream ss(str);
	int number;
	int i = 0;
	while (str[i])
	{
		if (!std::isdigit(str[i]))
			break;
		i++;
	}
	if ((int)str.size() != i)
		return -1;
	ss >> number;
	return (number);
}

void get_server_config(std::string line, int &i)
{
	// int flag(0);
	if (line[0] != '\t')
		i = 0;
	else
	{
		Server_Config server;
		std::stringstream ss(line);
		std::string key;
		std::string point;
		std::string value;
		int number(0);
		ss >> key;
		ss >> point;
		ss >> value;
		if (key == "host" && point == ":" && value == "localhost")
		{
			server.set_host(value);
		}
		else if (key == "port" && point == ":")
		{
			number = is_number(value);
			if (number == -1)
				throw("Invalide server_config port");
			server.set_port(number);
			
		}
		else if (key == "error_pages" && point == ":")
		{
			if (value.find(".html") == std::string::npos)
				throw("Invalide server_config error_page");
			std::vector<std::string> page;
			page.push_back(value);
			while (ss >> value)
			{
				if (value.find(".html") == std::string::npos)
					throw("Invalide server_config error_page");
				page.push_back(value);
			}
			server.set_error_pages(page);
		}
		else if (key == "client_body_limit" && point == ":")
		{
			number = is_number(value);
			if (number == -1)
				throw("Invalide server_config client_body_limit");
			server.set_client_body_limit(number);
		}
		else if (key == "route" && point.empty())
		{
			i = 2;
		}
		else
			throw("Invalide server_config");
	}
}

void get_server_route(std::string line, int &i)
{
	(void) i;
	if (line[0] != '\t' || (line[1] && line[1] != '\t'))
			throw("Invalide server_config route");
	std::stringstream ss;
	std::string key;
	std::string point;
	std::string value;
	ss >> key;
	ss >> point;
	ss >> value;
	Routes route;
	if (key == "path" && point == ":")
	{
		route.set_path(value);
		ss >> value;
		if (value.empty())
			throw("Invalide server_config multiple route path");

	}
	else if (key == "methods" && point == ":")
	{
		
	}
	else if (key == "default_file" && point == ":")
	{
		
	}
	else if (key == "directory_listing" && point == ":")
	{
		
	}
	else if (key == "cgi_bin" && point == ":")
	{
		
	}
	else if (key == "upload_enabled" && point == ":")
	{
		
	}
	else if (key == "upload_directory" && point == ":")
	{
		
	}
}

void Config::parse_config(std::string &config_file)
{
	std::stringstream ss(config_file);
	std::string line;
	size_t end;
	int i = 0;
	while(std::getline(ss, line))
	{
		end = line.find_last_not_of(" \t");
		if (end != std::string::npos)
			line.erase(end + 1);
		// std::cout << '|' <<line << '|' <<std::endl;
		if (line == "server")
			i = 1;
		if (i == 1)
			get_server_config(line, i);
		if (i == 2)
			get_server_route(line, i);
	}

}

void Config::print_config()
{
	for(size_t i = 0; i< servers.size(); i++)
	{
		std::cout << "Server " << i << std::endl;
		std::cout << "port: " << servers[i].get_port() << std::endl;
		std::cout << "host: " << servers[i].get_host() << std::endl;
		std::cout << "error_pages: ";
		for(size_t j = 0; j < servers[i].get_error_pages().size(); j++)
			std::cout << servers[i].get_error_pages()[j] << " ";
		std::cout << std::endl;
		std::cout << "client_body_limit: " << servers[i].get_client_body_limit() << std::endl;
		std::cout << "routes: " << std::endl;
		for(size_t j = 0; j < servers[i].get_routes().size(); j++)
		{
			std::cout << "\tpath: " << servers[i].get_routes()[j].get_path() << std::endl;
			std::cout << "\tdefault_file: " << servers[i].get_routes()[j].get_default_file() << std::endl;
			std::cout << "\tmethods: ";
			for(size_t k = 0; k < servers[i].get_routes()[j].get_methods().size(); k++)
				std::cout << servers[i].get_routes()[j].get_methods()[k] << " ";
			std::cout << std::endl;
			std::cout << "\tdirectory_listing: " << servers[i].get_routes()[j].get_directory_listing() << std::endl;
			std::cout << "\tupload_enabled: " << servers[i].get_routes()[j].get_upload_enabled() << std::endl;
			std::cout << "\tupload_directory: " << servers[i].get_routes()[j].get_upload_directory() << std::endl;
			std::cout << "\tcgi_bin: " << servers[i].get_routes()[j].get_cgi_bin() << std::endl;
		}
	}
}


/*

	SERVER CONFIG CLASS

*/
Server_Config::Server_Config()
{
	port = 0;
	client_body_limit = 0;
}

Server_Config::~Server_Config()
{

}

Server_Config::Server_Config(Server_Config const &src)
{
	*this = src;
}

Server_Config& Server_Config::Server_Config::operator=(Server_Config const &obj)
{
	if (this != &obj)
	{
		port = obj.port;
		host = obj.host;
		error_pages = obj.error_pages;
		client_body_limit = obj.client_body_limit;
		routes = obj.routes;
	}
	return *this;
}

int Server_Config::get_port()
{
	return port;
}

std::string Server_Config::get_host()
{
	return host;
}

std::vector<std::string> Server_Config::get_error_pages()
{
	return error_pages;
}

int Server_Config::get_client_body_limit()
{
	return client_body_limit;
}

std::vector<Routes> Server_Config::get_routes()
{
	return routes;
}

Server_Config& Server_Config::set_port(int port)
{
	this->port = port;
	return *this;
}

Server_Config& Server_Config::set_host(std::string &host)
{
	this->host = host;
	return *this;
}


Server_Config& Server_Config::set_error_pages(std::vector<std::string> &error_pages)
{
	this->error_pages = error_pages;
	return *this;
}

Server_Config& Server_Config::set_client_body_limit(int client_body_limit)
{
	this->client_body_limit = client_body_limit;
	return *this;
}

Server_Config& Server_Config::set_routes(std::vector<Routes> &routes)
{
	this->routes = routes;
	return *this;
}

/*

	ROUTES CLASS

*/

Routes::Routes()
{
	directory_listing = false;
	upload_enabled = false;
}

Routes::~Routes()
{

}

Routes::Routes(Routes const &src)
{
	*this = src;
}

Routes& Routes::Routes::operator=(Routes const &obj)
{
	if (this != &obj)
	{
		path = obj.path;
		default_file = obj.default_file;
		methods = obj.methods;
		directory_listing = obj.directory_listing;
		upload_enabled = obj.upload_enabled;
		upload_directory = obj.upload_directory;
		cgi_bin = obj.cgi_bin;
	}
	return *this;
}

std::string Routes::get_path()
{
	return path;
}

std::string Routes::get_default_file()
{
	return default_file;
}

std::vector<std::string> Routes::get_methods()
{
	return methods;
}

bool Routes::get_directory_listing()
{
	return directory_listing;
}

bool Routes::get_upload_enabled()
{
	return upload_enabled;
}

std::string Routes::get_upload_directory()
{
	return upload_directory;
}

std::string Routes::get_cgi_bin()
{
	return cgi_bin;
}

Routes& Routes::set_path(std::string path)
{
	this->path = path;
	return *this;
}

Routes& Routes::set_default_file(std::string default_file)
{
	this->default_file = default_file;
	return *this;
}

Routes& Routes::set_methods(std::vector<std::string> methods)
{
	this->methods = methods;
	return *this;
}

Routes& Routes::set_directory_listing(bool directory_listing)
{
	this->directory_listing = directory_listing;
	return *this;
}

Routes& Routes::set_upload_enabled(bool upload_enabled)
{
	this->upload_enabled = upload_enabled;
	return *this;
}

Routes& Routes::set_upload_directory(std::string upload_directory)
{
	this->upload_directory = upload_directory;
	return *this;
}

Routes& Routes::set_cgi_bin(std::string cgi_bin)
{
	this->cgi_bin = cgi_bin;
	return *this;
}

std::string Routes::get_redirect_url()
{
	return redirect_url;
}

Routes& Routes::set_redirect_url(std::string redirect_url)
{
	this->redirect_url = redirect_url;
	return *this;
}
