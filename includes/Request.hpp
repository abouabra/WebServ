#pragma once

#include <iostream>
#include <map>
#include "Utils.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
class Request {
private:
	std::string request_buff;

	std::string request_body;
	std::string method;
	std::string uri;
	std::string protocol;
	std::string host;
	std::string connection;
	std::string cache_control;
	std::string user_agent;
	std::string accept;
	std::string accept_encoding;
	std::string accept_language;
	std::string cookie;
	std::string transfer_encoding;


	std::string content_length;
	std::string content_type;
	std::string body;

	std::map<std::string , std::string> error_pages;
	std::map<std::string , std::string> mime_types;

	Server_Config server_config;
	Response response;

public:
	Request();
	~Request();
	Request(Request const &src);
	Request &operator=(Request const &obj);
	
	Response &get_response();

	void set_server_config(Server_Config config);
	Server_Config &get_server_config();

	void parse_request();
	void set_request_buff(std::string request_buff);
	void fill_info();

	
	std::string check_body(std::string);

	bool is_req_well_formed();
	int get_matched_location_for_request_uri();
	bool is_location_have_redirection(int index);
	bool is_method_allowded_in_location(int index);
	void handle_GET(int index);
	void handle_POST(int index);
	void handle_DELETE(int index);
	std::string get_requested_resource(int index);
	bool is_resource_exist(std::string path);
	bool is_resource_directory(std::string path);
	void handle_resource_directory(std::string path, int index);
	void handle_resource_file(std::string path, int index);
	void handle_directory_listing(std::string path, int index);
	bool is_resource_cgi(int index);
	void serve_cgi(int index, std::string path);
	void serve_file(std::string path, int index);
	bool if_location_support_upload(int index);
	void serve_upload(int index);
	void execute_cgi(std::string path_of_cgi_bin, char **argv);

};