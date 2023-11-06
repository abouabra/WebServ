#pragma once

#include <iostream>

class Response
{
private:
	std::string responce_buff;
public:
	Response();
	~Response();
	Response(Response const &src);
	Response &operator=(Response const &obj);

	Response& set_response_buff(std::string Response_buff);
	std::string get_response_buff();
};