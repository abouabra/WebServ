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
		client_addr = obj.client_addr;
		socket_fd = obj.socket_fd;
		timeout = obj.timeout;
	}
	return *this;
}

Client::Client(int socket_fd, struct sockaddr_in addr, int index)
{
	this->socket_fd = socket_fd;
	client_addr = addr;
	server_index = index;
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

int Client::get_server_index()
{
	return server_index;
}
Client &Client::set_timer(int timeout)
{
	this->timeout = timeout;
	return *this;
}

struct sockaddr_in Client::get_client_addr()
{
	return client_addr;
}

Request &Client::get_request()
{
	return request;
}

void Client::set_request(Request &request)
{
	this->request = request;
}