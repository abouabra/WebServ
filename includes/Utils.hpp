#pragma once
#include <iostream>
#include <map>
#include <sstream>

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
