/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 20:33:39 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/09 00:56:40 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
	Your executable will be run as follows:
	./ircserv <port> <password>

	port  		= 1 to 65535
				For TCP and UDP, a port number is a 16-bit unsigned integer, 
				thus ranging from 0 to 65535.
				For TCP, port number 0 is reserved and cannot be used.

	password    =  1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )
				; any 7-bit US_ASCII character,
				; except NUL, CR, LF, FF, h/v TABs, and " "
*/

int main(int argc, char **argv)
{
	// Handle invalid number of arguments
	if (argc != 3)
	{
		std::cerr << "ERROR: Invalid number of arguments" << std::endl;
		return (1);
	}
	
	// Try to run the server
	try
	{
		std::string port(argv[1]);
		std::string password(argv[2]);

		Server server(port, password);
		server.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	
	return (0);
}
