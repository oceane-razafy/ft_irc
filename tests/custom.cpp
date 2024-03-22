/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   custom.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 01:05:16 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/22 18:16:45 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	TO MAKE IT WORK
	> choose "ok" as server password
	> server name is ircserv
	> add your customs tests at line 270
		3 clients are already registered: oce, val, eli. 
		--> Example of a test: sendMsg("eli", fds, "PASS ok\r\n");
		=> eli sends "PASS ok\r\n" to the server
	
	* launch ./remake.sh or ./rebuild.sh to create and move executable in here
	* c++ custom.cpp && ./a.out 127.0.0.1 6667 (hostname + server port)
	* ./clean.sh (to clean everything (objs,bin, executables...)
*/

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

# define LIGHT_BLACK    "\e[38;5;238m"
# define LIGHT_RED      "\e[38;5;203m" 
# define LIGHT_GREEN    "\e[38;5;156m"
# define LIGHT_YELLOW   "\e[38;5;228m"
# define LIGHT_BLUE     "\e[38;5;111m"
# define LIGHT_PURPLE   "\e[38;5;183m"
# define LIGHT_CYAN     "\e[38;5;117m"
# define LIGHT_WHITE    "\e[38;5;255m"
# define LIGHT_ORANGE	"\e[38;5;223m"

# define GREEN	"\e[32m"
# define BRIGHT_CYAN	"\e[96m"
# define RESET	"\e[0m"

struct fds
{
	int oce;
	int val;
	int	eli;
};

std::string formatMsg(std::string message);

void	readData(std::string name, int fd)
{
	if (name == "oce")
		std::cout << LIGHT_BLUE;
	else if (name == "val")
		std::cout << LIGHT_GREEN;
	else
		std::cout << LIGHT_ORANGE;
	
	int bytesRead;
	char buffer[1024] = {0};
	while (true)
	{
		// std::cout << "BEFORE read" << std::endl;
		bytesRead = recv(fd, buffer, 1024, 0);
		// std::cout << "AFTER read" << std::endl;
		if (bytesRead == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
		{
			// std::cout << "read() -> EAGAIN or EWOULDBLOCK detected" << std::endl;
			break;
		}
		else if (bytesRead < 0)
		{
			perror("ERROR: Send failed");
			close(fd);
			exit(1);
		}
		if (bytesRead > 0)
		{
			if (name == "oce")
				std::cout << "\t[OCE received] ";
			else if (name == "val")
				std::cout << "\t[VAL received] ";
			else
				std::cout << "\t[ELI received] ";
			std::cout << formatMsg(std::string(buffer)) << std::endl;
			
		}
		// sleep(1);
		memset(buffer,0,1024);
	}
	std::cout << RESET << std::endl;
	usleep(10);
}


void	sendMsg(std::string name, fds fds, std::string msg)
{
	int fd;
	// std::cout << std::endl;
	if (name == "oce")
	{
		std::cout << LIGHT_BLUE;
		fd = fds.oce;
		std::cout << "[OCE] " << formatMsg(msg) << std::endl;
	}
	else if (name == "val")
	{
		std::cout << LIGHT_GREEN;
		fd = fds.val;
		std::cout << "[VAL] " << formatMsg(msg) << std::endl;
	}
	else
	{
		std::cout << LIGHT_ORANGE;
		fd = fds.eli;
		std::cout << "[ELI] " << formatMsg(msg) << std::endl;
	}
	
	if (send(fd, msg.c_str(), msg.size(), 0) < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			std::cout << "send() -> EAGAIN or EWOULDBLOCK detected" << std::endl;
		else
		{
			perror("ERROR: Send failed");
			close(fd);
			exit(1);
		}
	}
	
	usleep(10);

	if (name == "oce")
	{
		readData("oce", fds.oce);
		usleep(10);
		readData("val", fds.val);
		usleep(10);
		readData("eli", fds.eli);
	}
	else if (name == "val")
	{
		readData("val", fds.val);
		usleep(10);
		readData("oce", fds.oce);
		usleep(10);
		readData("eli", fds.eli);
	}
	else{
		usleep(10);
		readData("eli", fds.eli);
		usleep(10);
		readData("oce", fds.oce);
		usleep(10);
		readData("val", fds.val);
	}
	

	std::cout << RESET;
}


int	creationOfClient(int *fd, struct sockaddr_in *addr)
{
	timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
	
	// create the client socket
	if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
        perror("ERROR: Socket failed: ");
        return (-1);
    }

	// Connect the 2 clients to the server
    if (connect(*fd, (struct sockaddr*)&(*addr), sizeof(*addr)) < 0)
	{
    	perror("ERROR: Connect failed: ");
        return (-1);
    }

	// Make those 2 sockets non-blocking
	// fcntl(*fd, F_SETFL, O_NONBLOCK);

	// To make I/O operations non-blocking
    if (setsockopt(*fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
	{
		perror("ERROR: Setsockopt failed: ");
		return (-1);
	}

	if (setsockopt(*fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
	{
		perror("ERROR: Setsockopt failed: ");
		return (-1);
	}
	return (0);
}

// provide server IP and PORT as arguments
// --> ./a.out 127.0.0.1 6667
int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "ERROR: provide IP and PORT -> ex: ./a.out 127.0.0.1 6667" << std::endl;
		return (1);
	}

	fds fds;
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(std::atoi(argv[2]));
	// convert the address into binary
	
    if (inet_pton(AF_INET, argv[1], &addr.sin_addr) <= 0)
	{
        perror("ERROR: inet_pton failed: ");
        return (1);
    }

	// Creation of clients
	if (creationOfClient(&fds.oce, &addr) == -1)
		return (-1);
	if (creationOfClient(&fds.val, &addr) == -1)
		return (-1);
	if (creationOfClient(&fds.eli, &addr) == -1)
		return (-1);

//***********************************************************	
	// ------ Creation of client oce -----
	sendMsg("oce", fds, "PASS ok\r\n");
	sendMsg("oce", fds, "NICK oce\r\n");
	sendMsg("oce", fds, "USER oce oce 127.0.0.1 oce\r\n");

	// ------ Creation of client val -----
	sendMsg("val", fds, "PASS ok\r\n");
	sendMsg("val", fds, "NICK val\r\n");
	sendMsg("val", fds, "USER val val 127.0.0.1 val\r\n");

	// ------ Creation of client eli -----
	std::cout << std::endl;
	std::cout << "------------ creation of a new client ELI" << std::endl;
	sendMsg("eli", fds, "PASS ok\r\n");
	sendMsg("eli", fds, "NICK eli\r\n");
	sendMsg("eli", fds, "USER val val 127.0.0.1 val\r\n"); // on purpose that username is val (to test ERR_TOOMANYTARGETS)
//*************************************************************************

//////////////////////////////////////////////
// LAUNCH your custom tests here (3 clients are ready for your tests)

	std::cout << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "CUSTOM TESTS" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;


	std::cout << "-------- MODE +kw+l-v yourKey 0" << std::endl;
	std::cout << "> reset everything " << std::endl;

	sendMsg("oce", fds, "PASS\r\n");


///////////////////////////////////////////
	
	close(fds.oce);
	close(fds.val);
	close(fds.eli);
	
	return (0);
}

std::string formatMsg(std::string message)
{
	std::string formattedMessage;
	for(size_t i = 0; i < message.length(); i++)
	{
		switch (message[i])
		{
			case '\f':
				formattedMessage += "\\f";
				break ;
			case '\n':
				formattedMessage += "\\n";
				break ;
			case '\r':
				formattedMessage += "\\r";
				break ;
			case '\b':
				formattedMessage += "\\b";
				break ;
			case '\t':
				formattedMessage += "\\t";
				break ;
			case '\a':
				formattedMessage += "\\a";
				break ;
			case '\v':
				formattedMessage += "\\v";
				break ;
			case '\0':
				formattedMessage += "\\0";
				break ;
			default:
				formattedMessage += message[i];
		}
	}
	return formattedMessage;
}
