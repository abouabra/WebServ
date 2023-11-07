#pragma once

#include <iostream>
#include <map>
#include "Utils.hpp"
#include "Response.hpp"
#include "Config.hpp"
class Request {
private:
	std::string request_buff;

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

	std::map<int , std::string> error_pages;

	Server_Config *server_config;
	Response *response;

public:
	Request(std::string request_buff);
	~Request();
	Request(Request const &src);
	Request &operator=(Request const &obj);
	
	void set_server_config(Server_Config *config);
	Server_Config *get_server_config();

	void parse_request();
	void set_request_buff(std::string request_buff);
	// std::string get_response();
	void fill_info();

	std::string check_body(int error_code);
	
	void set_response(Response *response);
	Response *get_response();

	int is_req_well_formed();
	int get_matched_location_for_request_uri();
};