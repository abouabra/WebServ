#include "../includes/Config.hpp"
#include <cctype>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
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
	{
		servers = obj.servers;
		config_file = obj.config_file;
	}
	return *this;
}

Config::Config(std::string file_name)
{
	log("Config file: " + file_name, INFO);
	config_file = read_config(file_name);
	parse_config(config_file);
	// print_config();
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

void remove_comment(std::string &line)
{
	size_t end;
	end = line.find_first_of("#");
	if (end != std::string::npos)
		line.erase(end);
	end = line.find_last_not_of(" \t");
	if (end != std::string::npos)
		line.erase(end+1);
}

bool check_value(std::string value)
{
	std::string method[] = {"GET", "POST" , "PUT" , "PATCH" , "HEAD" ,"DELETE" , "CONNECT" , "OPTIONS" , "TRACE"};
	for (int i = 0; i < 9;i++)
	{
		if (value == method[i])
			return true;
	}
	return false;
}

bool check_duplicated(std::vector<std::string> method, std::string value)
{
	for (size_t i = 0; i < method.size(); i++)
	{
		if (method[i] == value)
			return false;
	}
	return true;
}

Routes get_server_route(std::stringstream &ss)
{
	Routes route;
	std::string line;
	while (std::getline(ss, line))
	{
		remove_comment(line);
		if (line.empty())
			continue;
		std::stringstream ss_2(line);
		std::string key;
		std::string point;
		std::string value;
		std::string dash;
		ss_2 >> dash;
		ss_2 >> key;
		ss_2 >> point;
		ss_2 >> value;
		if (line[0] != '\t' || (line[1] && line[1] != '\t'))
		{
			if (dash == "route" || dash == "server")
			{
				int i = ss.tellg();
				ss.seekg(i - line.size() - 1);
				break;
			}
			throw(std::runtime_error("Invalide server_config route"));
		}
		if (dash != "-" || value.empty() || point != ":")
			throw(std::runtime_error("Invalide server_config route"));
		if (key == "path")
		{
			route.set_path(value);
			ss_2 >> value;
			if (value.empty())
				throw(std::runtime_error("Invalide server_config multiple route path"));

		}
		else if (key == "methods")
		{
			do
			{
				if (check_value(value))
				{
					if (check_duplicated(route.get_methods(), value))
						route.get_methods().push_back(value);
					else
						throw(std::runtime_error("Invalide server_config method duplicated"));
				}
				else
					throw(std::runtime_error("Invalide server_config method"));
				ss_2 >> value;
				if (ss_2 && value != "|")
					throw(std::runtime_error("Invalide server_config method separator"));
			} while (ss_2 >> value);
		}
		else if (key == "default_file")
		{
			route.set_default_file(value);
			if (ss_2 >> value)
				throw(std::runtime_error("Invalide server_config default_file"));
		}
		else if (key == "directory_listing")
		{
			if (value == "false")
				route.set_directory_listing(false);
			else if (value == "true")
				route.set_directory_listing(true);
			else
				throw(std::runtime_error("Invalide server_config directory_listing"));
			if (ss_2 >> value)
				throw(std::runtime_error("Invalide server_config directory_listing"));
		}
		else if (key == "cgi_bin")
		{
			route.set_cgi_bin(value);
			if (ss_2 >> value)
				throw(std::runtime_error("Invalide server_config cgi_bin"));
		}
		else if (key == "cgi_extension")
		{
			route.set_cgi_extension(value);
			if (ss_2 >> value)
				throw(std::runtime_error("Invalide server_config cgi_extension"));
		}
		else if (key == "upload_enabled")
		{
			if (value == "false")
				route.set_upload_enabled(false);
			else if (value == "true")
				route.set_upload_enabled(true);
			else
				throw(std::runtime_error("Invalide server_config upload_enabled"));
			if (ss_2 >> value)
				throw(std::runtime_error("Invalide server_config upload_enabled"));
		}
		else if (key == "upload_directory")
		{
			route.set_upload_directory(value);
			if (ss_2 >> value)
				throw(std::runtime_error("Invalide server_config upload_directory"));
		}
		else if (key == "redirect_url")
		{
			route.set_redirect_url(value);
			if (ss_2 >> value)
				throw(std::runtime_error("Invalide server_config redirect_url"));
		}
	}
	return route;
}

int is_number(std::string &str)
{
	std::stringstream ss(str);
	int number;
	if (str.find_first_not_of("0123456789") != std::string::npos)
		throw(std::runtime_error("Invalide server_config port"));
	ss >> number;
	return (number);
}

Server_Config get_server_config(std::stringstream &ss)
{
	Server_Config new_serve;
	std::string line;
	while (std::getline(ss, line))
	{
		remove_comment(line);
		if(line == "server")
		{
			int i = ss.tellg();
			ss.seekg(i - line.size() - 1, std::ios::beg);
			break;
		}
		if (line[0] != '\t')
			throw(std::runtime_error("Invalide server_config indetation"));
		std::stringstream ss_2(line);
		std::string key;
		std::string point;
		std::string value;
		int number(0);
		ss_2 >> key;
		ss_2 >> point;
		ss_2 >> value;
		if (key.empty() || point.empty())
			throw(std::runtime_error("Invalide server_config syntax"));
		if (key == "host" && value == "localhost")
			new_serve.set_host(value);
		else if (key == "port" && ss_2)
		{
			number = is_number(value);
			new_serve.set_port(number);
		}
		else if (key == "root" && ss_2)
			new_serve.set_root(value);
		else if (key == "error_pages" && ss_2)
		{
			std::vector<std::string> page;
			do
			{
				if (value.find(".html") == std::string::npos)
				{
					throw(std::runtime_error("Invalide server_config error_page"));
				}
				page.push_back(value);
				ss_2 >> value;
				if (ss_2 && value != "|")
				{
					throw(std::runtime_error("Invalide server_config error_page"));
				}
			} while (ss_2 >> value);
			new_serve.set_error_pages(page);
		}
		else if (key == "client_body_limit")
		{
			number = is_number(value);
			if (number == -1)
				throw(std::runtime_error("Invalide server_config client_body_limit"));
			new_serve.set_client_body_limit(number);
		}
		else if (key == "route" && !ss_2)
		{
			Routes route;
			route = get_server_route(ss);
			new_serve.get_routes().push_back(route);
		}
		else if (key.empty())
			throw(std::runtime_error("Invalide server_config"));
	}
	return new_serve;
}

Server_Config& Server_Config::set_root(std::string root)
{
	this->root = root;
	return *this;
}

std::string Server_Config::get_root()
{
	return root;
}


void Config::parse_config(std::string &config_file)
{
	std::stringstream ss(config_file);
	std::string line;
	while(std::getline(ss, line))
	{
		if (line.empty())
			continue;
		remove_comment(line);
		if (line == "server")
		{
			Server_Config server;
			server = get_server_config(ss);
			servers.push_back(server);
		}
	}

}

void Config::print_config()
{
    for(size_t i = 0; i< servers.size(); i++)
    {
        std::cout << "Server id: " << i << std::endl;
        std::cout << "host: " << servers[i].get_host() << std::endl;
        std::cout << "port: " << servers[i].get_port() << std::endl;
        std::cout << "error_pages: ";
        for(size_t j = 0; j < servers[i].get_error_pages().size(); j++)
            std::cout << servers[i].get_error_pages()[j] << " ";
        std::cout << std::endl;
        std::cout << "client_body_limit: " << servers[i].get_client_body_limit() << std::endl;
        for(size_t j = 0; j < servers[i].get_routes().size(); j++)
        {
        	std::cout << "routes: " << std::endl;
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
            std::cout << "\tcgi_extension: " << servers[i].get_routes()[j].get_cgi_extension() << std::endl;
			std::cout << "\tredirect_url: " << servers[i].get_routes()[j].get_redirect_url() << std::endl;
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
		root = obj.root;
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

std::vector<Routes> &Server_Config::get_routes()
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
		redirect_url = obj.redirect_url;
		cgi_extension = obj.cgi_extension;
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


std::string Routes::get_cgi_extension()
{
	return cgi_extension;
}

Routes& Routes::set_cgi_extension(std::string cgi_extension)
{
	this->cgi_extension = cgi_extension;
	return *this;
}

std::vector<std::string>& Routes::get_methods()
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