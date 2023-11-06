#pragma once

#include <iostream>
#include "Utils.hpp"
class Request {
private:
	std::string request_buff;
	std::string result;
public:
	Request();
	Request(std::string request_buff);
	~Request();
	Request(Request const &src);
	Request &operator=(Request const &obj);

	void parse_request();
	std::string get_request_buff();
	void set_request_buff(std::string request_buff);
};