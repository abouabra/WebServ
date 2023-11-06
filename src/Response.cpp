#include "../includes/Response.hpp"

Response::Response()
{
}

Response::~Response()
{
}

Response::Response(Response const &src)
{
	*this = src;
}

Response &Response::operator=(Response const &obj)
{
	if (this != &obj)
		responce_buff = obj.responce_buff;
	return *this;
}

Response &Response::set_response_buff(std::string responce_buff)
{
	this->responce_buff = responce_buff;
	return *this;
}

std::string Response::get_response_buff()
{
	return responce_buff;
}
