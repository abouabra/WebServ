#pragma once
#include <iostream>
#include <cstring>
#include <sys/_types/_size_t.h>
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
	struct sockaddr_in client_addr; 
public:
	Client(int socket_fd, struct sockaddr_in addr);
	~Client();
	Client(Client const &src);
	Client &operator=(Client const &obj);


	int get_socket_fd();
	int get_timeout();
	struct sockaddr_in get_client_addr();
	int get_port();
	Client &set_timer(int timeout);

};