#ifndef EXCEPTIONS_HPP
#pragma once

#include <stdexcept>

namespace luapath{

struct  type_mismatch_exception
	: public std::exception
{
public:
	explicit type_mismatch_exception(const char *message)
		: m_Msg(message)
	{
	}
	explicit type_mismatch_exception(const std::string &message)
		: m_Msg(message)
	{
	}
	virtual ~type_mismatch_exception() throw()
	{
	}
	virtual const char* what() const throw() 
	{
		return m_Msg.c_str();
	}
protected:
	std::string m_Msg;

};

struct  path_lookup_exception
	: public std::exception
{
public:
	explicit path_lookup_exception(const char *message)
		: m_Msg(message)
	{
	}
	explicit path_lookup_exception(const std::string &message)
		: m_Msg(message)
	{
	}
	virtual ~path_lookup_exception() throw()
	{
	}

	virtual const char* what() const throw()
	{
		return m_Msg.c_str();
	}
protected:
	std::string m_Msg;

};

struct  lua_state_exception
	: public std::exception
{
public:
	explicit lua_state_exception(const char *message)
		: m_Msg(message)
	{
	}
	explicit lua_state_exception(const std::string &message)
		: m_Msg(message)
	{
	}
	virtual ~lua_state_exception() throw()
	{
	}

	virtual const char* what() const throw()
	{
		return m_Msg.c_str();
	}
protected:
	std::string m_Msg;

};

}
#endif // !EXCEPTIONS_HPP
