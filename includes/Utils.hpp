#pragma once
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>

enum log_level
{
	INFO,
	WARNING,
	ERROR,
	RESET
};

void log(std::string message, int level);

void guard(int status, std::string message);

std::string itoa(int i);

std::string read_file(std::string name);
