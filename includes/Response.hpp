#pragma once

#include <iostream>
#include <string>
#include "Utils.hpp"
class Response
{
private:
	std::string version;
	int status_code;
	std::string status_message;
	std::string ContentType;
	std::string body;
	std::string raw_response;
	std::string connection;
	
public:
	Response();
	~Response();
	Response(Response const &src);
	Response &operator=(Response const &obj);

	Response &set_version(std::string version);
	Response &set_status_code(int status_code);
	Response &set_content_type(std::string ContentType);
	Response &set_body(std::string body);
	Response &set_status_message(std::string status_message);
	Response &set_raw_response(std::string raw_response);
	Response &set_connection(std::string connection);
	Response &build_raw_response();
	std::string &get_raw_response();

};