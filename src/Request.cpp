#include "../includes/Request.hpp"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/_types/_size_t.h>
#include <unistd.h>

Request::Request()
{
	error_pages["400"] = " Bad Request";
	error_pages["401"] = " Unauthorized";
	error_pages["403"] = " Forbidden";
	error_pages["404"] = " Not Found";
	error_pages["405"] = " Method Not Allowed";
	error_pages["413"] = " Payload Too Large";
	error_pages["414"] = " URI Too Long";
	error_pages["500"] = " Internal Server Error";
	error_pages["501"] = " Not Implemented";
	error_pages["505"] = " HTTP Version Not Supported";
	error_pages["507"] = " Insufficient Storage";
	error_pages["510"] = " Not Extended";
	error_pages["511"] = " Network Authentication Required";
}
Request::~Request()
{
}

Request::Request(Request const &src): server_config(src.server_config), response(src.response)
{
	*this = src;
}

Request &Request::operator=(Request const &obj)
{
	if (this != &obj)
	{
		request_buff = obj.request_buff;
		method = obj.method;
		uri = obj.uri;
		protocol = obj.protocol;
		host = obj.host;
		connection = obj.connection;
		cache_control = obj.cache_control;
		user_agent = obj.user_agent;
		accept = obj.accept;
		accept_encoding = obj.accept_encoding;
		accept_language = obj.accept_language;
		cookie = obj.cookie;
		content_length = obj.content_length;
		content_type = obj.content_type;
		body = obj.body;
		transfer_encoding = obj.transfer_encoding;
		server_config = obj.server_config;
		response = obj.response;
		error_pages = obj.error_pages;
	}
	return *this;
}

void Request::set_server_config(Server_Config config)
{
	server_config = config;
}

Response& Request::get_response()
{
	return response;
}

void Request::set_request_buff(std::string request_buff)
{
	this->request_buff = request_buff;
}

Server_Config& Request::get_server_config()
{
	return server_config;
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
			uri = line.substr(method.length() + 1, line.find(" ", method.length() + 1) - method.length() - 1);
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
	int index;

	fill_info();
	if(!is_req_well_formed())
		return;
	index = get_matched_location_for_request_uri();
	if(index == -1)
		return;
	if(!is_location_have_redirection(index))
		return;
	if(!is_method_allowded_in_location(index))
		return;
	

	if(method == "GET")
		handle_GET(index);
	else if(method == "POST")
		handle_POST(index);
	else if(method == "DELETE")
		handle_DELETE(index);
	else
	{
		response.set_status_code(501)
			.set_content_type("text/html")
			.set_body(check_body( "error_pages/" + itoa(501) + ".html"))
			.build_raw_response();
	}
}

bool Request::is_req_well_formed()
{
	int status = 0;
	std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
	// std::string charset = "ABC";

	if(!transfer_encoding.empty()  && transfer_encoding != "chunked")
		status = 501;
	if(!transfer_encoding.empty() && !content_length.empty() && method == "POST")
		status = 400;
	if(uri.find_first_not_of(charset) != std::string::npos)
		status = 400;
	if(uri.size() > 2048)
		status = 414;
	if((int)uri.size() > server_config.get_client_body_limit())
		status = 413;
	
	if(status)
	{
		response.set_status_code(status)
			.set_body(check_body( "error_pages/" + itoa(status) + ".html"))
			.set_content_type("text/html")
			.build_raw_response();
		return false;
	}
	return true;
}

std::string Request::check_body(std::string path)
{
	std::string error_code = path.substr(path.find_last_of("/") + 1);
	for(size_t j = 0; j < server_config.get_error_pages().size(); j++)
	{
		if(server_config.get_error_pages()[j] == error_code)
		{
			return read_file(server_config.get_root() + path);
		}
	}
	error_code = error_code.substr(0, 3);
	std::string standard = "<html><title>" + error_code + error_pages[error_code] + "</title><body style=\"color: green;background: #000\" ><h1 style=\"text-align: center;\">ERROR: " + error_code + error_pages[error_code] + "</h1></body></html>";
	return standard;
}


int Request::get_matched_location_for_request_uri()
{
	std::string location;

	location = uri.substr(0, uri.find_last_of("/"));
	if(location.empty())
		location = "/";
	for(size_t i = 0; i < server_config.get_routes().size(); i++)
	{
		if(location == server_config.get_routes()[i].get_path())
			return i;
	}

	response.set_status_code(404)
		.set_body(check_body( "error_pages/" + itoa(404) + ".html"))
		.set_content_type("text/html")
		.build_raw_response();
	return -1;
}

bool Request::is_location_have_redirection(int index)
{
	std::string redirect_url = server_config.get_routes()[index].get_redirect_url();
	std::string location = uri.substr(0, uri.find_last_of("/") + 1);
	std::string url = "http://localhost:" + itoa(server_config.get_port());

	if(!redirect_url.empty() && location == uri)
	{
		redirect_url.insert(0, "/");
		response.set_raw_response("HTTP/1.1 301 Moved Permanently\r\nLocation: " + url + redirect_url + "\r\n\r\n");
		return false;
	}
	return true;
}

bool Request::is_method_allowded_in_location(int index)
{
	for(int i = 0; i < (int)server_config.get_routes()[index].get_methods().size(); i++)
	{
		if(server_config.get_routes()[index].get_methods()[i] == method)
			return true;
	}
	response.set_status_code(405)
		.set_content_type("text/html")
		.set_body(check_body( "error_pages/" + itoa(405) + ".html"))
		.build_raw_response();
	return false;
}

void Request::handle_GET(int index)
{
	std::string path = server_config.get_routes()[index].get_path();
	if(path == "/")
		path = "";
	if(uri == (path + "/"))
		path += "/" + server_config.get_routes()[index].get_default_file();
	else
	{
		if(path == "/")
			path = "";
		path += uri.substr(uri.find_last_of("/"));
	}
	std::cout << "path: " << path << std::endl;
	path = server_config.get_root() + path;
	std::cout << "path: " << path << std::endl;


	if(access(path.c_str(), F_OK))
	{
		response.set_status_code(404)
			.set_content_type("text/html")
			.set_body(check_body( "error_pages/" + itoa(404) + ".html"))
			.build_raw_response();
		return;
	}


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

	std::string res = "HTTP/1.1 200 OK\r\nContent-Type: "+mime_type+"\r\n\r\n";
	std::string res_body = read_file(path);
	res.append(res_body);
	response.set_raw_response(res);
}

void Request::handle_POST(int index)
{
	(void) index;
}

void Request::handle_DELETE(int index)
{
	(void) index;
}