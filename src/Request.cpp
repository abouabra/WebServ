#include "../includes/Request.hpp"
#include <sstream>
#include <string>

Request::~Request()
{
}

Request::Request(Request const &src): server_config(src.server_config), response_obj(src.response_obj)
{
	*this = src;
}

Request &Request::operator=(Request const &obj)
{
	if (this != &obj)
	{
		request_buff = obj.request_buff;
		response = obj.response;
	}
	return *this;
}

Request::Request(std::string request_buff)
{
	this->request_buff = request_buff;
	response_obj = NULL;
	server_config = NULL;

	error_pages[400] = " Bad Request";
	error_pages[401] = " Unauthorized";
	error_pages[403] = " Forbidden";
	error_pages[404] = " Not Found";
	error_pages[405] = " Method Not Allowed";
	error_pages[413] = " Payload Too Large";
	error_pages[414] = " URI Too Long";
	error_pages[500] = " Internal Server Error";
	error_pages[501] = " Not Implemented";
	error_pages[505] = " HTTP Version Not Supported";
	error_pages[507] = " Insufficient Storage";
	error_pages[510] = " Not Extended";
	error_pages[511] = " Network Authentication Required";
}

void Request::set_response_obj(Response *response_obj)
{
	this->response_obj = response_obj;
}

Response *Request::get_response_obj()
{
	return response_obj;
}

void Request::set_server_config(Server_Config *config)
{
	server_config = config;
}

void Request::set_request_buff(std::string request_buff)
{
	this->request_buff = request_buff;
}

std::string Request::get_response()
{
	return response;
}

void Request::fill_info()
{
	std::stringstream ss(request_buff);
	std::string line;
	while(std::getline(ss, line))
	{
		if(line.find("HTTP/1.1") != std::string::npos)
		{
			method = line.substr(0, line.find(" "));
			uri = line.substr(line.find("GET ") + 4, line.find(" HTTP/1.1") - 4);
			protocol = "HTTP/1.1";
		}
		else if(line.find("Host: ") != std::string::npos)
			host = line.substr(line.find("Host: ") + 6);
		else if(line.find("Connection: ") != std::string::npos)
			connection = line.substr(line.find("Connection: ") + 12);
		else if(line.find("Cache-Control: ") != std::string::npos)
			cache_control = line.substr(line.find("Cache-Control: ") + 15);
		else if(line.find("User-Agent: ") != std::string::npos)
			user_agent = line.substr(line.find("User-Agent: ") + 12);
		else if(line.find("Accept: ") != std::string::npos)
			accept = line.substr(line.find("Accept: ") + 8);
		else if(line.find("Accept-Encoding: ") != std::string::npos)
			accept_encoding = line.substr(line.find("Accept-Encoding: ") + 17);
		else if(line.find("Accept-Language: ") != std::string::npos)
			accept_language = line.substr(line.find("Accept-Language: ") + 17);
		else if(line.find("Cookie: ") != std::string::npos)
			cookie = line.substr(line.find("Cookie: ") + 8);
		else if(line.find("Content-Length: ") != std::string::npos)
			content_length = line.substr(line.find("Content-Length: ") + 16);
		else if(line.find("Content-Type: ") != std::string::npos)
			content_type = line.substr(line.find("Content-Type: ") + 14);
		else if(line.find("Body: ") != std::string::npos)
			body = line.substr(line.find("Body: ") + 6);
		else if(line.find("Transfer-Encoding: ") != std::string::npos)
			transfer_encoding = line.substr(line.find("Transfer-Encoding: ") + 19);
	}
}

void Request::parse_request()
{
	// std::cout << "Request: " << request_buff << std::endl;
	// std::cout << request_buff << std::endl;
	fill_info();
	
	if(!is_req_well_formed())
		return;
	
	

	
	
	
	
	
	
	
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

	response = "HTTP/1.1 200 OK\r\nContent-Type: "+mime_type+"\r\n\r\n";
	path = "assets/static" + path;
	// std::cout << "path: " << path << std::endl;
	std::string response_body = read_file(path);
	response.append(response_body);
	response_obj->set_raw_response(response);
}

int Request::is_req_well_formed()
{
	if(!transfer_encoding.empty()  && transfer_encoding != "chunked")
	{
		response_obj->set_status_code(501)
			.set_body(check_body(501))
			.set_content_type("text/html")
			.build_raw_response();
		return 0;
	}
	if(!transfer_encoding.empty() && !content_length.empty() && method == "POST")
	{
		response_obj->set_status_code(400)
			.set_body(check_body(400))
			.set_content_type("text/html")
			.build_raw_response();
		return 0;
	}
	std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	if(uri.find_first_not_of(charset) != std::string::npos)
	{
		response_obj->set_status_code(400)
			.set_body(check_body(400))
			.set_content_type("text/html")
			.build_raw_response();
		return 0;
	}
	if(uri.size() > 2048)
	{
		response_obj->set_status_code(414)
			.set_body(check_body(414))
			.set_content_type("text/html")
			.build_raw_response();
		return 0;
	}
	if((int)uri.size() > server_config->get_client_body_limit())
	{
		response_obj->set_status_code(413)
			.set_body(check_body(413))
			.set_content_type("text/html")
			.build_raw_response();
		return 0;
	}
	return 1;
}

std::string Request::check_body(int error_code)
{
	for(size_t j = 0; j < server_config->get_error_pages().size(); j++)
	{
		if(server_config->get_error_pages()[j] == itoa(error_code))
		{
			std::string path = "assets/static" + server_config->get_error_pages()[j];
			return read_file(path);
		}
	}

	std::string standard = "<html><title>" + itoa(error_code) + error_pages[error_code] + "</title><body style=\"color: green;background: #000\" > \
		<h1 style=\"text-align: center;\">ERROR: " + itoa(error_code) + error_pages[error_code] + "</h1></body></html>";
	return standard;
}
