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

	std::map<std::string , std::string> error_pages;

	Server_Config server_config;
	Response response;

public:
	Request();
	Request(std::string request_buff);
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
};