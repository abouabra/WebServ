#include "../includes/Request.hpp"

Request::Request()
{
}

Request::~Request()
{
}

Request::Request(Request const &src)
{
	*this = src;
}

Request &Request::operator=(Request const &obj)
{
	if (this != &obj)
	{
		request_buff = obj.request_buff;
		result = obj.result;
	}
	return *this;
}

Request::Request(std::string request_buff)
{
	this->request_buff = request_buff;
}

void Request::parse_request()
{
	// std::cout << "Request: " << request_buff << std::endl;
	std::string path =  request_buff.substr(request_buff.find("GET ") + 4, request_buff.find(" HTTP/1.1") - 4);
	if(path == "/")
		path = "/index.html";

	std::string extention = path.substr(path.find_last_of(".") + 1);
	std::string mime_type;
	if(extention == "html")
		mime_type = "text/html";
	else if(extention == "css")
		mime_type = "text/css";
	else if(extention == "js")
		mime_type = "application/javascript";
	else if(extention == "jpg")
		mime_type = "image/jpeg";
	else if(extention == "png")
		mime_type = "image/png";
	else
		mime_type = "text/plain";

	result = "HTTP/1.1 200 OK\r\nContent-Type: "+mime_type+"\r\n\r\n";
	path = "assets/static" + path;
	// std::cout << "path: " << path << std::endl;
	std::string response_body = read_file(path);
	result.append(response_body);
}

void Request::set_request_buff(std::string request_buff)
{
	this->request_buff = request_buff;
}

std::string Request::get_request_buff()
{
	return result;
}
