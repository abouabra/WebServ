#ifndef ReversePolishNotation_HPP
#define ReversePolishNotation_HPP

#include <iostream>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>

class ReversePolishNotation
{
	private:
		std::string operations;
		ReversePolishNotation();
	public:
		ReversePolishNotation(std::string operations);
		~ReversePolishNotation();
		ReversePolishNotation(const ReversePolishNotation &other);
		ReversePolishNotation &operator=(const ReversePolishNotation &other);
		void calculate();
};

#endif