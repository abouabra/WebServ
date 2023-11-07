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

	std::string response;
	std::map<int , std::string> error_pages;
public:
	Request();
	Request(std::string request_buff);
	~Request();
	Request(Request const &src);
	Request &operator=(Request const &obj);

	void parse_request(Server_Config &config);
	void set_request_buff(std::string request_buff);
	std::string get_response();
	void fill_info();

	int is_req_well_formed(Server_Config &config);
	Response response_obj;

	std::string check_body(Server_Config &config , int error_code);
	// Config config;
};