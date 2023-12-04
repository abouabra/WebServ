#pragma once
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <cstring>
#include <netinet/in.h>

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

std::string ft_trim(std::string str, std::string charset);

int is_valid_ip(std::string ip);

int ft_atoi(std::string str);

int count_num(std::string str, char c);

void check_multiple(std::string &value, std::stringstream &ss_2);