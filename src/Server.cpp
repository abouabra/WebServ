#include "../includes/Server.hpp"
#include "../includes/Utils.hpp"
#include <unistd.h>

/*

	SERVER CLASS

*/

Server::Server(): config(Config("config/default.conf"))
{
	
}

Server::Server(Config &config) : config(config)
{
	max_fd = 0;
	for(size_t i = 0;i < config.servers.size(); i++)
	{
		int server_fd;

		server_fd = set_up_server(config.servers[i]);
		server_fds.push_back(server_fd);
	}
	run_server();
}

Server::~Server()
{
	
}

Server::Server(Server const &src): config(src.config)
{
	*this = src;
}

Server& Server::Server::operator=(Server const &obj)
{
	if (this != &obj)
	{
		server_fds = obj.server_fds;
		clients = obj.clients;
		master = obj.master;
		reads = obj.reads;
		writes = obj.writes;
	}
	return *this;
}
struct sockaddr_in Server::set_up_addr(int port)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	// addr.sin_addr.s_addr = inet_addr("1.2.3.4"); // cant be done since we cannot change our ip
	return addr;
}

int Server::set_up_server(Server_Config &config)
{
	int server_fd;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	guard(server_fd, "Error creating socket");

	set_to_non_blocking(server_fd);

	int reuse = 1; // Enable SO_REUSEADDR
	int status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	guard(status , "Error setting socket options");

	struct sockaddr_in addr = set_up_addr(config.get_port());
	status = bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));

	guard(status, "Error binding socket");

	status = listen(server_fd, 1024);
	guard(status, "Error listening on socket");

	log("listening on 0.0.0.0:" + itoa(config.get_port()), INFO);
	if(server_fd > max_fd)
		max_fd = server_fd;

	return server_fd;
}

void Server::set_up_fd_sets()
{
	FD_ZERO(&master);
	FD_ZERO(&reads);
	FD_ZERO(&writes);
	for(size_t i = 0; i < server_fds.size(); i++)
		FD_SET(server_fds[i], &master);
}

void Server::run_server()
{
	set_up_fd_sets();

	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	while(true)
	{
		reads = master;
		int status = select(max_fd + 1, &reads, &writes, NULL, &timeout);
		if(status == -1)
		{
			log("Error selecting", ERROR);
			continue;
		}

		// check for new connections
		for(size_t i = 0; i < server_fds.size(); i++)
		{
			if(FD_ISSET(server_fds[i], &reads))
				accept_new_connection(server_fds[i], i);
		}

		// check for data from clients
		for(size_t i = 0; i < clients.size(); i++)
		{

			if(check_for_timeout(clients[i], i))
				continue;

			if(FD_ISSET(clients[i].get_socket_fd(), &reads))
			{
				if(read_from_client(clients[i], i))
					continue;
			}
			if(FD_ISSET(clients[i].get_socket_fd(), &writes))
			{
				if(write_to_client(clients[i], i))
					continue;
				
			}
		}
	}
}

void Server::set_to_non_blocking(int client_fd)
{
	int status = fcntl(client_fd, F_GETFL, 0);
	if(status == -1)
	{
		log("Error getting socket flags", ERROR);
		return;
	}
	status = fcntl(client_fd, F_SETFL, status | O_NONBLOCK);
	if(status == -1)
		log("Error setting socket flags", ERROR);
}

void Server::accept_new_connection(int server_fd, int index)
{
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	
	int client_fd = accept(server_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_addr_len);
	if(client_fd == -1)
	{
		log("Error accepting connection", ERROR);
		return;
	}
	if(client_fd > max_fd)
		max_fd = client_fd;

	set_to_non_blocking(client_fd);

	FD_SET(client_fd, &master);
	FD_SET(client_fd, &reads);
	
	Client client(client_fd, client_addr, index);
	client.set_timer(time(NULL));
	clients.push_back(client);
	log("New connection on: " + itoa(client_fd), INFO);
}

void Server::close_connection(Client &client, int index)
{
	FD_CLR(client.get_socket_fd(), &master);
	FD_CLR(client.get_socket_fd(), &writes);
	close(client.get_socket_fd());
	clients.erase(clients.begin() + index);
	log("Closing connection on: " + itoa(client.get_socket_fd()), INFO);
}

int Server::check_for_timeout(Client &client, int index)
{
	if(time(NULL) - client.get_timeout() > 5)
	{
		close_connection(client, index);
		return 1;
	}
	return 0;
}

int Server::read_from_client(Client &client, int i)
{
	client.set_timer(time(NULL));
	int size = 1024;
	char buffer[size];
	std::memset(buffer, 0, sizeof(buffer));

	// log("Reading from socket: " + itoa(client.get_socket_fd()), WARNING);
	int bytes_read = recv(client.get_socket_fd(), buffer, size, 0);
	if(bytes_read == -1)
		return 1;
	if(bytes_read == 0)
	{
		close_connection(client, i);
		return 1;
	}
	FD_SET(client.get_socket_fd(), &writes);
	// FD_CLR(client.get_socket_fd(), &reads);
	Request request(buffer);
	client.set_request(request);
	client.get_request().parse_request(config.servers[client.get_server_index()]);
	return 0;
}

int Server::write_to_client(Client &client, int index)
{
	client.set_timer(time(NULL));
	// std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, World!</h1></body></html>";
	std::string response = client.get_request().response_obj.get_raw_response();
	// std::cout << response << std::endl;
	// log("Sending to socket: " + itoa(client.get_socket_fd()), WARNING);
	int bytes_sent = send(client.get_socket_fd(), response.c_str(), response.length(), 0);
	if(bytes_sent == -1)
		return 1;
	close_connection(client, index);
	return 0;
}