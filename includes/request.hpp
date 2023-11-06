#pragma once

#include <iostream>
#include "Utils.hpp"
class Request {
private:
	std::string request_buff;
	std::string method;
	std::string uri;
	std::string http_version;
	std::string response;
public:
	Request();
	Request(std::string request_buff);
	~Request();
	Request(Request const &src);
	Request &operator=(Request const &obj);

	void parse_request();
	void set_request_buff(std::string request_buff);
	std::string get_response();
};