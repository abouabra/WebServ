#include "../includes/Server.hpp"
#include "../includes/Utils.hpp"
#include <ctime>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

/*

	SERVER CLASS

*/

Server::Server(Config config): conf(config)
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

Server::Server(Server const &src): conf(src.conf)
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
struct sockaddr_in Server::set_up_addr(int port, std::string host)
{
	struct sockaddr_in addr;
    struct sockaddr_in *tmp;
    struct addrinfo *result = 0;
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	int i = getaddrinfo(host.c_str(), itoa(port).c_str(), &hints, &result);
	guard(i, gai_strerror(i));//test latter
	tmp = reinterpret_cast<sockaddr_in*>(result->ai_addr);
	std::memmove(&addr, tmp, sizeof(*tmp));
	freeaddrinfo(result);
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

	struct sockaddr_in addr = set_up_addr(config.get_port(), config.get_host());
	status = bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));

	guard(status, "Error binding socket");

	status = listen(server_fd, 1024);
	guard(status, "Error listening on socket");
	log("listening on " + addr_to_ip(addr.sin_addr.s_addr) + ':' + itoa(config.get_port()), INFO);
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
	timeout.tv_sec = 3;
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
	int status = fcntl(client_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
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

	Client client(client_fd, client_addr);
	client.set_server_config(conf.servers[index]);
	client.set_timer(time(NULL));
	clients.push_back(client);
	log("New connection on: " + addr_to_ip(client_addr.sin_addr.s_addr) + ':' + itoa(client_addr.sin_port) + " socket: " + itoa(client_fd), INFO);
}

void Server::close_connection(Client &client, int index)
{
	log("Closing connection on: " + itoa(client.get_socket_fd()), INFO);
	FD_CLR(client.get_socket_fd(), &master);
	FD_CLR(client.get_socket_fd(), &writes);
	close(client.get_socket_fd());
	clients.erase(clients.begin() + index);
}

int Server::check_for_timeout(Client &client, int index)
{
	if(time(NULL) - client.get_timeout() > client.get_request().get_time())
	{
		std::cout << "------timeout-------"<<std::endl;
		close_connection(client, index);
		return 1;
	}
	return 0;
}

int Server::read_from_client(Client &client, int i)
{
	int size = 1024;
	char buffer[size];
	client.set_timer(time(NULL));

	// log("Reading from socket: " + itoa(client.get_socket_fd()), WARNING);
	std::string total;
	while(true)
	{
		std::memset(buffer, 0, sizeof(buffer));
		int bytes_read = recv(client.get_socket_fd(), buffer, size, 0);
		if(bytes_read == -1)
			return 1;
		if(bytes_read == 0)
		{
			close_connection(client, i);
			return 1;
		}
		total.append(buffer, bytes_read);
		if(bytes_read < size)
			break;
	}
	// std::cout << total << std::endl;
	client.get_request().set_request_buff(total);
	client.get_request().set_server_config(client.get_server_config());
	client.get_request().parse_request();

	// FD_CLR(client.get_socket_fd(), &master);//to hande keep-alive
	FD_SET(client.get_socket_fd(), &writes);
	return 0;
}

int Server::write_to_client(Client &client, int index)
{
	(void) index;
	client.set_timer(time(NULL));
	std::string response_body = client.get_request().get_response().get_raw_response();
	// log("Sending to socket: " + itoa(client.get_socket_fd()), WARNING);
	int bytes_sent = send(client.get_socket_fd(), response_body.c_str(), response_body.length(), 0);
	if(bytes_sent == -1)
		return 1;
	if (client.get_request().get_connection() == "close")
		close_connection(client, index);
	else
		FD_CLR(client.get_socket_fd(), &writes);
	// close_connection(client, index);
	return 0;
}