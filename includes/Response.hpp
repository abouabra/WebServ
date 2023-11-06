#pragma once

#include <iostream>
#include <string>
#include "Utils.hpp"
class Response
{
private:
	std::string version;
	int status_code;
	std::string ContentType;
	std::string headers;
	std::string body;
	std::string raw_response;
	
public:
	Response();
	~Response();
	Response(Response const &src);
	Response &operator=(Response const &obj);
	Response &set_version(std::string version);
	Response &set_status_code(int status_code);
	Response &set_content_type(std::string ContentType);
	Response &set_body(std::string body);
	Response &set_raw_response(std::string raw_response);
	void build_raw_response();
	std::string get_raw_response();

};