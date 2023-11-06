#include <algorithm>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>

#include <sstream>
#include <string>
#include <sys/errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <vector>
#include <map>

#define PORT 2003
enum log_level
{
	INFO,
	WARNING,
	ERROR,
	RESET
};

std::string itoa(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

void guard(int status, std::string message)
{
	message = message + "\n" + strerror(errno);
	if(status < 0)
		throw std::runtime_error(message);
}

std::map<int, std::string> log_colors;

void log(std::string message, int level)
{
	time_t current;
	char buffer[100];
	(void) level;
	current = time(NULL);
	std::memset(buffer, 0, sizeof(buffer));
	strftime(buffer, 100, "[%Y-%m-%d %H:%M:%S]", localtime(&current));

	std::cout << log_colors[level] <<  buffer << " " << message << log_colors[RESET] <<std::endl;
}

bool is_in(int fd, std::vector<int> fds)
{
	for(size_t i = 0; i < fds.size(); i++)
	{
		if(fd == fds[i])
			return true;
	}
	return false;
}

std::string read_file(std::string name)
{
	std::fstream file(name.c_str(), std::ios::in);
	std::string content;
	std::string line;

	while(std::getline(file, line))
		content += line + "\n";
	return content;
}

struct Client {
  int socket_fd;
  time_t timeout;
};

void run_server()
{
	std::vector<int> server_socket_fds;
	std::vector<struct sockaddr_in> server_addrs;
	std::map<int, Client> clients;

	int number_of_servers = 3;
	for (int i = 0; i < number_of_servers; ++i)
	{
		struct sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(PORT + i);
		log("listening on 0.0.0.0:" + itoa(PORT + i), INFO);
		server_addr.sin_addr.s_addr = INADDR_ANY;
		server_addrs.push_back(server_addr);
	}

	int status;
	for(int i= 0 ;i < number_of_servers; i++)
	{
		int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		guard(socket_fd, "Error creating socket");
		int reuse = 1; // Enable SO_REUSEADDR
		status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
		guard(status , "Error setting socket options");

		status = bind(socket_fd, reinterpret_cast<sockaddr *>(&server_addrs[i]), sizeof(server_addrs[i]));
		guard(status, "Error binding socket");

		status = listen(socket_fd, 1024);
		guard(status, "Error listening on socket");

		server_socket_fds.push_back(socket_fd);
	}
	fd_set master;
	fd_set reads;
	fd_set writes;
	FD_ZERO(&writes);
	FD_ZERO(&master);
	for(int i = 0; i < number_of_servers; i++)
		FD_SET(server_socket_fds[i], &master);

	std::cout << std::endl;
	while(true)
	{
		reads = master;
		FD_ZERO(&writes);
		timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		status = select(FD_SETSIZE, &reads, &writes, NULL, &timeout);
		guard(status, "Error selecting from socket");

		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);

		time_t current_time = time(NULL);
		for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end();it++) {
			if (current_time - it->second.timeout > 5) {
				std::cout << "Client: " << it->first << " timeout" << std::endl;
				close(it->second.socket_fd);
				FD_CLR(it->second.socket_fd, &master);
				clients.erase(it);
			}
		}
		//check for new connections
		for (int k = 0; k < number_of_servers; k++)
		{
			if (FD_ISSET(server_socket_fds[k], &reads))
			{
				int client_fd = accept(server_socket_fds[k], reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
				guard(client_fd, "Error accepting connection");
				int flags = fcntl(client_fd, F_GETFL, 0);
				status = fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
				guard(status, "Error setting flags");

				// std::cout << "open client: " << client_fd << std::endl;
				log("open client: " + itoa(client_fd), INFO);
				FD_SET(client_fd, &master);

				Client client;
				client.socket_fd = client_fd;
				client.timeout = time(NULL);
				clients[client_fd] = client;
			}
		}
		//check for new data
		std::string total_req;

		for(int i=0; i<FD_SETSIZE; i++)
		{
			if(FD_ISSET(i, &reads) && !is_in(i, server_socket_fds))
			{
					char buffer[700000];
					std::memset(buffer, 0, sizeof(buffer));
					// std::cout << "read from client: " << i << std::endl;
					log("read from client: " + itoa(i), INFO);
					int bytes_read = 0;
					while(true)
					{
						bytes_read = recv(i, buffer, sizeof(buffer), 0);
						// std::cout << "bytes_read: " << bytes_read << std::endl;
						if (bytes_read == 0)
							break;
						if (bytes_read < 0)
							break;
						// std::cout << buffer << std::endl;
						total_req.append(buffer, bytes_read);
						std::memset(buffer, 0, sizeof(buffer));
					}
					if(!bytes_read)
					{
						// std::cout << "close client: " << i << std::endl;
						log("close client: " + itoa(i), INFO);
						close(i);
						FD_CLR(i, &master);
						std::map<int, Client>::iterator it = clients.find(i);
						clients.erase(it);
					}

					// std::cout << total_req << std::endl;
					FD_SET(i, &writes);
				  	clients[i].timeout = time(NULL);
			}
			if(FD_ISSET(i, &writes) && !is_in(i, server_socket_fds))
			{
					// std::string buf(buffer);
					// std::string port = buf.substr(buf.find("Host: localhost:") + 16, 4);
					// std::cout << "write to client: " << i << std::endl;

					log("write to client: " + itoa(i), INFO);
					std::string path =  total_req.substr(total_req.find("GET ") + 4, total_req.find(" HTTP/1.1") - 4);
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

					std::string response = "HTTP/1.1 200 OK\r\nContent-Type: "+mime_type+"\r\n\r\n";
					// std::stringstream ss;
					// ss << i;
					// std::string index = ss.str();

					path = "assets/static" + path;
					std::cout << "path: " << path << std::endl;
					std::string response_body = read_file(path);
					// std::string response_body = "<html><head></head><body><h1>Hello, World!</h1></body></html>";
					
					response.append(response_body);
					
					int bytes_sent = send(i, response.c_str(), response.length(), 0);
					guard(bytes_sent, "Error sending response");

					// std::cout << "close client: " << i << std::endl;
					log("close client: " + itoa(i), INFO);

					close(i);
					FD_CLR(i, &master);
					FD_CLR(i, &writes);
					std::map<int, Client>::iterator it = clients.find(i);
					clients.erase(it);
			}
		}
	}
}
int main()
{
	log_colors[INFO] = "\033[1;32m";
	log_colors[WARNING] = "\033[1;33m";
	log_colors[ERROR] = "\033[1;31m";
	log_colors[RESET] = "\033[0m";
	try
	{
		run_server();
	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}