#include "../includes/Response.hpp"

Response &Response::set_version(std::string version)
{
	this->version = version;
	return *this;
}

Response &Response::set_status_code(int status_code)
{
	this->status_code = status_code;
	return *this;
}

Response &Response::set_content_type(std::string ContentType)
{
	this->ContentType = ContentType;
	return *this;
}

Response &Response::set_body(std::string body)
{
	this->body = body;
	return *this;
}

Response& Response::build_raw_response()
{
	this->raw_response = this->version + " " + itoa(this->status_code) + "\r\n";
	this->raw_response += "Content-Type: " + this->ContentType + "\r\n";
	this->raw_response += "Content-Length: " + itoa(this->body.length()) + "\r\n";
	this->raw_response += "\r\n";
	this->raw_response += this->body;
	return *this;
}


Response::Response()
{
	this->version = "HTTP/1.1";
	this->status_code = 404;
	this->ContentType = "text/html";
	this->headers = "";
	this->body = "";
	this->raw_response = "";
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
	{
		version = obj.version;
		status_code = obj.status_code;
		ContentType = obj.ContentType;
		headers = obj.headers;
		body = obj.body;
		raw_response = obj.raw_response;
	}
	return *this;
}

std::string Response::get_raw_response()
{
	return this->raw_response;
}

Response &Response::set_raw_response(std::string raw_response)
{
	this->raw_response = raw_response;
	return *this;
}
