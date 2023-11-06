#pragma once

#include <sys/errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <vector>
#include <iostream>
#include "Client.hpp"
#include "Config.hpp"

class Server {
private:
	Server();

public:
	Server(Config &config);
	~Server();
	Server(Server const &src);
	Server &operator=(Server const &obj);

	std::vector<int> server_fds;
	
	std::vector<Client> clients;

	fd_set master;
	fd_set reads;
	fd_set writes;

	int max_fd;

	int set_up_server(Server_Config &config);
	struct sockaddr_in set_up_addr(int port);
	void set_up_fd_sets();
	void run_server();
	void accept_new_connection(int server_fd);
	void close_connection(Client &client, int index);
	void set_to_non_blocking(int client_fd);

};