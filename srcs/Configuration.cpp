/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 21:34:08 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/11 21:17:03 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configuration.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

const char *Configuration::InvalidPortException::what() const throw()
{
	/*
		For TCP and UDP, a port number is a 16-bit unsigned integer, 
		thus ranging from 0 to 65535.
		For TCP, port number 0 is reserved and cannot be used
	*/
	return ("ERROR: A TCP port number has to be between 1 and 65535");
}

const char *Configuration::EmptyPasswordException::what() const throw()
{
	return ("ERROR: Password is empty");
}

Configuration::Configuration(const std::string &port, const std::string &password)
{
	int portNb;
	
	if (!(Utils::isAllCharactersDigit(port)) 
		|| !(std::istringstream(port) >> portNb)
		|| ((portNb <= 0 || portNb > 65535)))
			throw InvalidPortException();
	_port = portNb;
	
	if (password.empty())
		throw EmptyPasswordException();
	Utils::checkPasswordKeyValidity(password);
	_password = password;
}

int	Configuration::getPort(void) const
{
	return (_port);
}

const std::string	Configuration::getPassword(void) const
{
	return (_password);
}
