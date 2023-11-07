#pragma once
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <iostream>
#include <cstring>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>

class Client {
private:
	int socket_fd;
	int timeout;
	struct sockaddr_in client_addr; 

	Server_Config server_config;
	Request	request;
public:
	Client(int socket_fd, struct sockaddr_in addr);
	~Client();
	Client(Client const &src);
	Client &operator=(Client const &obj);

	int get_socket_fd();
	int get_timeout();

	void set_server_config(Server_Config config);
	Server_Config get_server_config();
	
	struct sockaddr_in get_client_addr();
	int get_port();
	Client &set_timer(int timeout);

	Request &get_request();
};