#pragma once
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <cstring>

enum log_level
{
	INFO,
	WARNING,
	ERROR,
	RESET
};

void log(std::string message, int level);

void guard(int status, std::string message);

std::string  addr_to_ip(in_addr_t number);

std::string itoa(int i);

std::string read_file(std::string name);

std::string urlDecode(std::string str);

char **make_argv(std::string request_body, std::string cgi_bin_path, std::string cgi_bin);

void free_arr(char **arr);