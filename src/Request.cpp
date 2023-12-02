#include "../includes/Request.hpp"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

Request::Request()
{
	status_message["100"] = " Continue";
	status_message["101"] = " Switching Protocols";
	status_message["102"] = " Processing";
	status_message["200"] = " OK";
	status_message["201"] = " Created";
	status_message["202"] = " Accepted";
	status_message["203"] = " Non-Authoritative Information";
	status_message["204"] = " No Content";
	status_message["205"] = " Reset Content";
	status_message["206"] = " Partial Content";
	status_message["207"] = " Multi-Status";
	status_message["300"] = " Multiple Choices";
	status_message["301"] = " Moved Permanently";
	status_message["302"] = " Found";
	status_message["303"] = " See Other";
	status_message["304"] = " Not Modified";
	status_message["400"] = " Bad Request";
	status_message["401"] = " Unauthorized";
	status_message["403"] = " Forbidden";
	status_message["404"] = " Not Found";
	status_message["405"] = " Method Not Allowed";
	status_message["406"] = " Not Acceptable";
	status_message["407"] = " Proxy Authentication Required";
	status_message["408"] = " Request Timeout";
	status_message["409"] = " Conflict";
	status_message["413"] = " Payload Too Large";
	status_message["414"] = " URI Too Long";
	status_message["500"] = " Internal Server Error";
	status_message["501"] = " Not Implemented";
	status_message["505"] = " HTTP Version Not Supported";
	status_message["507"] = " Insufficient Storage";
	status_message["510"] = " Not Extended";
	status_message["511"] = " Network Authentication Required";


	mime_types["html"] = "text/html";
	mime_types["css"] = "text/css";
	mime_types["js"] = "application/javascript";
	mime_types["jpg"] = "image/jpeg";
	mime_types["png"] = "image/png";
	mime_types["txt"] = "text/plain";
	mime_types["zip"] = "application/zip";
	mime_types["mp4"] = "video/mp4";
	mime_types["mp3"] = "audio/mpeg";
	mime_types["json"] = "application/json";
	time_out = 3;
	connection = "closed";
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
		status_message = obj.status_message;
		mime_types = obj.mime_types;
		request_body = obj.request_body;
		time_out = obj.time_out;
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
	// std::cout << "request_buff: " << request_buff << std::endl;
	std::stringstream ss(request_buff);
	std::string line;
	bool is_body = false;
	// bool is_body = false;
	while(std::getline(ss, line))
	{
		if(line.find("\r") != std::string::npos)
			line.replace(line.find_last_of("\r"), 1, "");
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
		if (line.empty())
		{
			is_body = true;
			break;
		}
	}
	request_body.clear();
	if (is_body)
	{
		while(std::getline(ss, line))
		{
			// std::cout << "im here" << std::endl;
			request_body += line;
			if(ss.eof())
				break;
			request_body += "\n";
		}
		// request_body = urlDecode(request_body);
	}
	if(uri.find("?") != std::string::npos) //need to check if body is url encoded reminder
	{
		// std::cout << content_type << std::endl;
		std::string query_string = uri.substr(uri.find("?") + 1);
		uri = uri.substr(0, uri.find("?"));
		std::stringstream ss(query_string);
		std::string token;
		while(std::getline(ss, token, '&'))
		{
			request_body += token + "\n";
		}
		request_body = urlDecode(request_body);
	}
	// std::cout << "request_body: " << request_body << std::endl;
	// std::cout << "Cookie: " << cookie << std::endl;
}

void Request::handelCookies(int index)
{
	(void) index;
	// std::vector<Cookies>::iterator it = cookies_data.begin();
	// while (it != cookies_data.end())
	// {
	// 	if (cookie == it->id)
	// 	{
	// 		if (server_config.get_routes()[index].get_path().find("login"))
	// 			// std::cout << server_config.get_routes()[index].get_path()<<std::endl;
	// 	}
	// 	it++;
	// }

	std::stringstream ss(cookie);
	std::string token;

	while(std::getline(ss, token, ';'))
	{
		if (token.find("password=") != std::string::npos)
			cookies_data.password = token.substr(token.find("password=") + 9);
		else if (token.find("username=") != std::string::npos)
			cookies_data.username = token.substr(token.find("username=") + 9);
	}
}

std::string Request::get_connection()
{
	return connection;
}
void Request::set_connection(std::string str)
{
	connection = str;
}

void Request::parse_request()
{
	int index;

	fill_info();
	if(!is_req_well_formed())
		return;
	index = get_matched_location_for_request_uri();
	
	// if(index == -1)
	// 	return;
	if(is_location_have_redirection(index))
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
			.set_connection(connection)
			.set_body(check_body( "error_pages/" + itoa(501) + ".html"))
			.set_status_message(status_message[itoa(501)])
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
	if(transfer_encoding.empty() && content_length.empty() && method == "POST")
		status = 400;
	if(uri.find_first_not_of(charset) != std::string::npos)
		status = 400;
	if(uri.size() > 2048)
		status = 414;
	if((int)request_body.size() > server_config.get_client_body_limit())
		status = 413;
	
	if(status)
	{
		response.set_status_code(status)
			.set_body(check_body( "error_pages/" + itoa(status) + ".html"))
			.set_connection(connection)
			.set_content_type("text/html")
			.set_status_message(status_message[itoa(status)])
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
			return read_file(server_config.get_root() + "/" + path);
		}
	}
	error_code = error_code.substr(0, 3);
	std::string standard = "<html><title>" + error_code + status_message[error_code] + "</title><body style=\"color: green;background: #000\" ><h1 style=\"text-align: center;\">" + error_code + status_message[error_code] + "</h1></body></html>";
	return standard;
}


int Request::get_matched_location_for_request_uri()
{
	std::string location;

	location = uri.substr(0, uri.find_last_of("/"));
	if(location.empty())
		location = "/";
	// std::cout << "location: " << location << std::endl;
	for(size_t i = 0; i < server_config.get_routes().size(); i++)
	{
		if(location == server_config.get_routes()[i].get_path())
			return i;
	}
	return -1;
}

bool Request::is_location_have_redirection(int index)
{
	if (index == -1)
		return false;
	std::string redirect_url = server_config.get_routes()[index].get_redirect_url();
	std::string location = uri.substr(0, uri.find_last_of("/") + 1);

	if(!redirect_url.empty() && location == uri)
	{
		redirect_url.insert(0, "/");
		// std::cout << "rediret to: " << redirect_url << std::endl;
		response.set_raw_response("HTTP/1.1 301 Moved Permanently\r\nLocation: " + redirect_url + "\r\nContent-Type: text/html\r\nContent-Length: 0\r\nConnection: " + connection + "\r\n\r\n");

		return true;
	}
	return false;
}

bool Request::is_method_allowded_in_location(int index)
{
	if (index == -1)
		return true;
	for(int i = 0; index != -1 && i < (int)server_config.get_routes()[index].get_methods().size(); i++)
	{
		if(server_config.get_routes()[index].get_methods()[i] == method)
			return true;
	}
	response.set_status_code(405)
		.set_content_type("text/html")
		.set_connection(connection)
		.set_body(check_body( "error_pages/" + itoa(405) + ".html"))
		.set_status_message(status_message[itoa(405)])
		.build_raw_response();
	return false;
}

void Request::handle_GET(int index)
{
	std::string path = get_requested_resource(index);
	if(!is_resource_exist(path))
		return;
	
	if(is_resource_directory(path))
		handle_resource_directory(path, index);
	else
		handle_resource_file(path, index);

	// std::cout << "path: " << path << std::endl;
}

std::string Request::get_requested_resource(int index)
{
	if (index == -1)
		return server_config.get_root() + uri;
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
	// std::cout << "path: " << path << std::endl;
	path = server_config.get_root() + path;
	// std::cout << "path: " << path << std::endl;
	return path;
}

bool Request::is_resource_exist(std::string path)
{
	// std::cout << "path: " << path << std::endl;
	if(access(path.c_str(), F_OK))
	{
		response.set_status_code(404)
			.set_content_type("text/html")
			.set_connection(connection)
			.set_body(check_body( "error_pages/" + itoa(404) + ".html"))
			.set_status_message(status_message[itoa(404)])
			.build_raw_response();
		return false;
	}
	return true;
}

bool Request::is_resource_directory(std::string path)
{
	struct stat s;
	stat(path.c_str(),&s);
	if( s.st_mode & S_IFDIR )
		return true;
	return false;
}

void Request::handle_resource_directory_for_DELETE(std::string path, int index)
{
	if (index == -1)
	{
		response.set_status_code(403)
			.set_content_type("text/html")
			.set_connection(connection)
			.set_body(check_body( "error_pages/" + itoa(403) + ".html"))
			.set_status_message(status_message[itoa(403)])
			.build_raw_response();
		return;
	}
	// std::cout << "path: " << path << std::endl;
	if(path[path.length() - 1] != '/')
	{
		response.set_status_code(409)
			.set_content_type("text/html")
			.set_connection(connection)
			.set_body(check_body( "error_pages/" + itoa(409) + ".html"))
			.set_status_message(status_message[itoa(409)])
			.build_raw_response();
	}
	if(!server_config.get_routes()[index].get_cgi_bin().empty())
		serve_cgi(index, path);
	else
		delete_item(path);
}

void Request::handle_resource_directory(std::string path, int index)
{
	if (index == -1)
	{
		response.set_status_code(403)
			.set_content_type("text/html")
			.set_connection(connection)
			.set_body(check_body( "error_pages/" + itoa(403) + ".html"))
			.set_status_message(status_message[itoa(403)])
			.build_raw_response();
		return;
	}
	// std::cout << "path: " << path << std::endl;
	if(path[path.length() - 1] != '/')
	{
			// std::cout << "rediret to: " << uri << std::endl;
			response.set_raw_response("HTTP/1.1 301 Moved Permanently\r\nLocation: " +  uri + "/\r\nContent-Type: text/html\r\nContent-Length: 0\r\nConnection: " + connection + "\r\n\r\n");
			return;
	}
	if(server_config.get_routes()[index].get_directory_listing() && !server_config.get_routes()[index].get_path().empty())
		handle_directory_listing(path, index);
	else
	{
		response.set_status_code(403)
			.set_content_type("text/html")
			.set_connection(connection)
			.set_body(check_body( "error_pages/" + itoa(403) + ".html"))
			.set_status_message(status_message[itoa(403)])
			.build_raw_response();
	}
}

void Request::handle_resource_file(std::string path, int index)
{
	if (!cookie.empty() && path.find("logged_in") == std::string::npos)
	{
		// handelCookies(index);
		path = "/login/logged_in.html";
		response.set_raw_response("HTTP/1.1 302 Found\r\nLocation: " +  path + "\r\nContent-Type: text/html\r\nContent-Length: 0\r\nConnection: " + connection + "\r\n\r\n");
			return;
	}
	if(is_resource_cgi(index, path))
		serve_cgi(index, path);
	else
		serve_file(path, index);
}

void Request::handle_directory_listing(std::string path, int index)
{
	(void) index;
	std::string res_body = "<html><title>Directory listing for " + uri + "</title><body><h1>Directory listing for " + uri + "</h1><hr><ul>";
	
	DIR *dir;
	struct dirent *ent;
	dir = opendir (path.c_str());
	while ((ent = readdir (dir)) != NULL)
	{
		if(ent->d_name[0] != '.')
			res_body += "<li><a href=\"" + uri + ent->d_name + "\">" + ent->d_name + "</a></li>";
	}
	closedir (dir);
	res_body += "</ul><hr></body></html>";
	
	response.set_status_code(200)
		.set_content_type("text/html")
		.set_connection(connection)
		.set_status_message(status_message[itoa(200)])
		.set_body(res_body)
		.build_raw_response();
}

bool Request::is_resource_cgi(int index, std::string path)
{

	if (index == -1)
		return false;
	size_t pos = path.find_last_of(".");
	if(pos == std::string::npos)
		return false;
	std::string extention = path.substr(pos);
	// std::cout << "index: " << index << std::endl;
	// std::cout << "extention: " << server_config.get_routes()[index].get_cgi_extension() << std::endl;
	if(!server_config.get_routes()[index].get_cgi_bin().empty() && !server_config.get_routes()[index].get_cgi_extension().empty() && extention == server_config.get_routes()[index].get_cgi_extension())
		return true;
	return false;
}

void Request::serve_cgi(int index, std::string cgi_script_path)
{
	// std::cout << "request_body: " << request_body << std::endl;
	std::string cgi_bin_path = server_config.get_routes()[index].get_cgi_bin();
	char **argv  = make_argv(request_body, cgi_bin_path, cgi_script_path);
	char **envp = make_envp();
	// for(int i = 0; argv[i]; i++)
	// 	std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
	// std::cout << "cgi_bin_path: " << cgi_bin_path << std::endl;
	// for(int i = 0; envp && envp[i]; i++)
	// 	std::cout << "envp[" << i << "]: " << envp[i] << std::endl;
	execute_cgi(cgi_bin_path, argv, envp);
	free_arr(argv);
	if(envp)
		free_arr(envp);
}
char **Request::make_envp()
{
	if(cookie.empty())
		return NULL;
	std::string username = "username=" + cookies_data.username;
	std::string password = "password=" + cookies_data.password;
	char **envp = new char*[3];
	envp[0] = strdup(username.c_str());
	envp[1] = strdup(password.c_str());
	envp[2] = NULL;
	return envp;
}
void Request::execute_cgi(std::string path_of_cgi_bin, char **argv, char **envp)
{
	int pipe_fds[2];
	int pid;
	int status;
	
	if(pipe(pipe_fds) < 0)
	{
		response.set_status_code(500)
			.set_content_type("text/html")
			.set_connection(connection)
			.set_body(check_body( "error_pages/" + itoa(500) + ".html"))
			.set_status_message(status_message[itoa(500)])
			.build_raw_response();
		return;
	}
	pid = fork();
	if(pid < 0)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		log("fork failed", ERROR);
		response.set_status_code(500)
			.set_content_type("text/html")
			.set_connection(connection)
			.set_body(check_body( "error_pages/" + itoa(500) + ".html"))
			.set_status_message(status_message[itoa(500)])
			.build_raw_response();
		return;
	}

	if(pid == 0)
	{
		close(pipe_fds[0]);
		dup2(pipe_fds[1], 1);
		close(pipe_fds[1]);
		execve(path_of_cgi_bin.c_str(), argv, envp);
		log("execve failed", ERROR);
		exit(69);
	}
	else
	{
		close(pipe_fds[1]);
		time_t start_time = time(NULL);
		while (waitpid(pid, &status, WNOHANG) == 0)
		{
			if (time(NULL) - start_time > time_out)
			{
				kill(pid, SIGTERM);
				close(pipe_fds[0]);
				log("CGI script timeout", ERROR);
				response.set_status_code(500)
					.set_content_type("text/html")
					.set_connection(connection)
					.set_body(check_body( "error_pages/" + itoa(500) + ".html"))
					.set_status_message(status_message[itoa(500)])
					.build_raw_response();
				return;
			}
		}
		int exit_status = status << 8;
		if(exit_status != 0)
		{
			log("CGI script returned non-zero exit status", ERROR);
			response.set_status_code(500)
				.set_content_type("text/html")
				.set_connection(connection)
				.set_body(check_body( "error_pages/" + itoa(500) + ".html"))
				.set_status_message(status_message[itoa(500)])
				.build_raw_response();
			return;
		}
		char buffer[1024];
		std::string res_body;
		while(true)
		{
			std::memset(buffer, 0, sizeof(buffer));
			int bytes_read = read(pipe_fds[0], buffer, sizeof(buffer));
			if(bytes_read == -1)
			{
				log("read failed", ERROR);
				response.set_status_code(500)
					.set_content_type("text/html")
					.set_connection(connection)
					.set_body(check_body( "error_pages/" + itoa(500) + ".html"))
					.set_status_message(status_message[itoa(500)])
					.build_raw_response();
				return;
			}
			if(bytes_read == 0)
				break;
			res_body.append(buffer, bytes_read);
		}
		close(pipe_fds[0]);
		response.set_raw_response(res_body);
		// response.set_status_code(200)
		// 	.set_content_type("text/html")
		// 	.set_connection(connection)
		// 	.set_body(res_body)
		// 	.set_status_message(status_message[itoa(200)])
		// 	.build_raw_response();
	}
}

void Request::serve_file(std::string path, int index)
{
	(void) index;
	size_t pos = path.find_last_of(".");
	if(pos == std::string::npos)
	{
		response.set_status_code(403)
			.set_content_type("text/html")
			.set_connection(connection)
			.set_body(check_body( "error_pages/" + itoa(403) + ".html"))
			.set_status_message(status_message[itoa(403)])
			.build_raw_response();
		return;
	}
	std::string extention = path.substr(pos + 1);
	response.set_status_code(200)
		.set_content_type(mime_types[extention])
		.set_connection(connection)
		.set_body(read_file(path))
		.set_status_message(status_message[itoa(200)])
		.build_raw_response();
}

void Request::handle_POST(int index)
{
	if(if_location_support_upload(index))
	{
		// std::cout << "im here" << std::endl;
		serve_upload(index);
		return;
	}
	// std::cout << "request_body: " << request_body << std::endl;

	std::string path = get_requested_resource(index);
	// std::cout << "path: " << path << std::endl;
	// std::cout << "index: " << index << std::endl;
	if(!is_resource_exist(path))
		return;

	if(is_resource_directory(path))
		handle_resource_directory(path, index);
	else
	{
		if(is_resource_cgi(index, path))
			serve_cgi(index, path);
		else
		{
			response.set_status_code(403)
				.set_content_type("text/html")
				.set_connection(connection)
				.set_body(check_body( "error_pages/" + itoa(403) + ".html"))
				.set_status_message(status_message[itoa(403)])
				.build_raw_response();
		}
	}
}

void Request::handle_DELETE(int index)
{
	std::string path = get_requested_resource(index);
	if(index == -1)
	{
		delete_item(path);
		return;
	}
	if(is_resource_directory(path))
		handle_resource_directory_for_DELETE(path, index);
	else
	{
		if(is_resource_cgi(index, path))
			serve_cgi(index, path);
		else
			delete_item(path);
	}
}


bool Request::if_location_support_upload(int index)
{
	if (index == -1)
		return false;
	if(server_config.get_routes()[index].get_upload_enabled())
		return true;
	return false;
}

void Request::serve_upload(int index)
{
	(void) index;
	// std::cout << request_body << std::endl;
	std::string filename = request_body.substr(request_body.find("filename=\"") + 10);
	filename = filename.substr(0, filename.find("\""));
	// std::cout << "filename: " << filename << std::endl;

	std::string body = request_body.substr(request_body.find("\r\n\r\n") + 4);
	//body = body.substr(0, body.find("\r\n-----"));
	//?????
	//file path change to custem path
	// std::string file_path = server_config.get_routes()[index].get_upload_directory() + "/" + filename;
	std::string file_path = server_config.get_root() + "/" + server_config.get_routes()[index].get_upload_directory() + "/" + filename;
	// std::cout << "file_path: " << file_path << std::endl;
	// std::cout << "upload to " + file_path << std::endl;
	std::ofstream file;
	file.open(file_path.c_str(), std::ios::out | std::ios::binary);
	if(!file.is_open())
	{
		response.set_status_code(500)
			.set_content_type("text/html")
			.set_connection(connection)
			.set_body(check_body( "error_pages/" + itoa(500) + ".html"))
			.set_status_message(status_message[itoa(500)])
			.build_raw_response();
		return;
	}
	file << body;
	file.close();
	response.set_status_code(201)
		// .set_content_type(request_body.substr(request_body.find("Content-Type: ") + 14))//type of upload file
		.set_connection(connection)
		.set_body(check_body( "error_pages/" + itoa(201) + ".html"))
		.set_status_message(status_message[itoa(201)])
		.build_raw_response();
}

void Request::delete_item(std::string path)
{
	// std::cout << "here i am: " << path << std::endl;
	if(std::system(("rm -rf " + path).c_str()) != 0)
	{
		if(has_write_acces_on_folder(path))
		{
			response.set_status_code(500)
				.set_content_type("text/html")
				.set_connection(connection)
				.set_body(check_body( "error_pages/" + itoa(500) + ".html"))
				.set_status_message(status_message[itoa(500)])
				.build_raw_response();
			return;
		}
		else
		{
			response.set_status_code(403)
				.set_content_type("text/html")
				.set_connection(connection)
				.set_body(check_body( "error_pages/" + itoa(403) + ".html"))
				.set_status_message(status_message[itoa(403)])
				.build_raw_response();
			return;
		}
	}
	response.set_status_code(204)
		.set_content_type("text/html")
		.set_connection(connection)
		.set_body(check_body( "error_pages/" + itoa(204) + ".html"))
		.set_status_message(status_message[itoa(204)])
		.build_raw_response();
}

unsigned int	Request::get_time()
{
	return time_out;
}

void Request::set_time(unsigned int tm)
{
	time_out = tm;
}
bool Request::has_write_acces_on_folder(std::string path)
{
	struct stat s;
	stat(path.c_str(),&s);
	if( s.st_mode & S_IWUSR )
		return true;
	return false;
}