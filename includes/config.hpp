#pragma once
#include <iostream>
#include <vector>
#include <fstream>

class Routes {
private:
	std::string path;
	std::string default_file;
	std::vector<std::string> methods;
	
	bool directory_listing;

	bool upload_enabled;
	std::string upload_directory;

	std::string cgi_bin;

public:
	Routes();
	~Routes();
	Routes(Routes const &src);
	Routes &operator=(Routes const &obj);

	std::string get_path();
	std::string get_default_file();
	std::vector<std::string> get_methods();
	bool get_directory_listing();
	bool get_upload_enabled();
	std::string get_upload_directory();
	std::string get_cgi_bin();

	Routes &set_path(std::string path);
	Routes &set_default_file(std::string default_file);
	Routes &set_methods(std::vector<std::string> methods);
	Routes &set_directory_listing(bool directory_listing);
	Routes &set_upload_enabled(bool upload_enabled);
	Routes &set_upload_directory(std::string upload_directory);
	Routes &set_cgi_bin(std::string cgi_bin);
};

class Server_Config {
private:
	int port;
	std::string host;
	std::vector<std::string> error_pages;
	int client_body_limit;
	std::vector<Routes> routes;
public:
	Server_Config();
	~Server_Config();
	Server_Config(Server_Config const &src);
	Server_Config &operator=(Server_Config const &obj);

	int get_port();
	std::string get_host();
	std::vector<std::string> get_error_pages();
	int get_client_body_limit();
	std::vector<Routes> get_routes();

	Server_Config &set_port(int port);
	Server_Config &set_host(std::string &host);
	Server_Config &set_error_pages(std::vector<std::string> &error_pages);
	Server_Config &set_client_body_limit(int client_body_limit);
	Server_Config &set_routes(std::vector<Routes> &routes);
};

class Config {
private:
	Config();
	std::string config_file;

public:
	~Config();
	Config(std::string file_name);
	Config(Config const &src);
	Config &operator=(Config const &obj);

	std::string read_config(std::string &file_name);
	void parse_config(std::string &config_file);
	std::vector<Server_Config> servers;
};