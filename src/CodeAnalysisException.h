#pragma once
#include <exception>
#include <sstream>

#include "SourceText.h"

class CodeAnalysisException : public std::exception
{
public:
	CodeAnalysisException(std::string message, SourceText::Iterator pos)
	{
		std::stringstream ss;
		ss << "(" << pos.row << ", " << pos.column << "): error: " << message;
		this->message = ss.str();
	}
	char const* what() const override;
private:
	std::string message;
};

inline char const* CodeAnalysisException::what() const
{
	return message.c_str();
}
