#include "../includes/Client.hpp"
#include <iostream>

Client::Client()
{
}

Client::~Client()
{
}

Client::Client(Client const &src)
{
	*this = src;
}

Client &Client::operator=(Client const &obj)
{
	if (this != &obj)
	{
		socket_fd = obj.socket_fd;
		timeout = obj.timeout;
	}
	return *this;
}

Client::Client(int socket_fd, struct sockaddr_in addr)
{
	this->socket_fd = socket_fd;
	client_addr = addr;
	timeout = 0;
}

int Client::get_port()
{
	return ntohs(client_addr.sin_port);
}

int Client::get_socket_fd()
{
	return socket_fd;
}

int Client::get_timeout()
{
	return timeout;
}

Client &Client::set_timer(int timeout)
{
	this->timeout = timeout;
	return *this;
}
