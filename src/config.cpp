#include "../includes/Config.hpp"
#include <exception>
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

void Config::parse_config(std::string &config_file)
{
	(void) config_file;
}

void Config::print_config()
{
	
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
