#include "../includes/Client.hpp"
#include <cstddef>
#include <iostream>

Client::~Client()
{
}

Client::Client(Client const &src): server_config(src.server_config)
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

Client::Client(int socket_fd, struct sockaddr_in addr)
{
	this->socket_fd = socket_fd;
	client_addr = addr;
	timeout = 0;
	server_config = NULL;
	request = NULL;
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

void Client::set_server_config(Server_Config &config)
{
	server_config = &config;
}

Server_Config *Client::get_server_config()
{
	return server_config;
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
	return *request;
}

void Client::set_request(Request &request)
{
	this->request = &request;
}
