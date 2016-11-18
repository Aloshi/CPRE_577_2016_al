#pragma once

#include <sstream>
#include <exception>

class GLException : public std::exception
{
public:
	virtual const char* what() const noexcept override
	{
		return mMsg.c_str();
	}

	template <typename T>
	GLException& operator<<(T rhs)
	{
		std::stringstream ss;
		ss << mMsg << rhs;
		mMsg = ss.str();
		return *this;
	}

private:
	std::string mMsg;
};
