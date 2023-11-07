#pragma once
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
	Client();
	int socket_fd;
	int timeout;
	int server_index;
	struct sockaddr_in client_addr; 
	Request	request;
public:
	Client(int socket_fd, struct sockaddr_in addr, int index);
	~Client();
	Client(Client const &src);
	Client &operator=(Client const &obj);


	int get_socket_fd();
	int get_timeout();
	int get_server_index();
	struct sockaddr_in get_client_addr();
	int get_port();
	Client &set_timer(int timeout);

	Request &get_request();
	void set_request(Request &request);
};