/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 01:05:16 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/21 00:51:06 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	TO MAKE IT WORK
	> choose "ok" as server password
	> server name is ircserv
	
	* launch ./remake.sh or ./rebuild.sh to create and move executable in here
	* c++ tests.cpp && ./a.out 127.0.0.1 6667 (hostname + server port)
	* ./clean.sh (to clean everything (objs,bin, executables...)

	NOTE:
		If ALL (or too many) tests are launched, redirect the results
		* FOR THIS CLIENT SIDE: c++ tests.cpp && ./a.out 127.0.0.1 6667 > res_client 2>&1
		* FOR SERVER SIDE: ./remake.sh && valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6667 ok > res_server 2>&1
		(you can remove valgrind if not needed)
		
		To read the results, execute this commands
		* more res_client
		* more res_server
	
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
#include <string.h>

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

void		testIsRegistered(fds fds); // in PASS, NICK, USER

void		testPass(fds fds);
void		testNick(fds fds);
void		testUser(fds fds);
void 		testChannelNames(fds fds);
void 		testKey(fds fds);
void 		testInvite(fds fds);
void		testChannelIsFullLogic(fds fds);
void		testOtherJoinLogic(fds fds);
void		testPrivmsg(fds fds);
void		testKick(fds fds);
void		testTopic(fds fds);
void		testMode(fds fds);
void		testPart(fds fds);
void		testPing(fds fds);
void		testChannelNameCaseInsensitive(fds fds);
void		testPartialData(fds fds);
void		testCrazyMsg(fds fds);
void		testNicknamesChanged(fds fds);

void		testQuit(fds fds);

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
			if (errno == EBADF)
				break;
			perror("ERROR: Recv failed");
			close(fd);
			exit(1);
		}
		else if (bytesRead == 0)
		{
			std::cerr << "CLIENT SIDE: Client has quit" << std::endl;
			close(fd);
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

	// std::cout << "BEFORE send" << std::endl;
	
	if (send(fd, msg.c_str(), msg.size(), 0) < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			std::cout << "send() -> EAGAIN or EWOULDBLOCK detected" << std::endl;
		else
		{
			if (errno == EBADF)
			{
				if (name == "oce")
					std::cout << "[OCE IS NOT ON THE SERVER]"<< std::endl;
				else if (name == "val")
					std::cout << "[VAL IS NOT ON THE SERVER]"<< std::endl;
				else
					std::cout << "[ELI IS NOT ON THE SERVER]"<< std::endl;
			}
			else
			{
				perror("ERROR: Send failed");
				close(fd);
				exit(1);
			}
		}
	}
	// std::cout << "AFTER send" << std::endl;
	
	usleep(10);

	if (name == "oce")
	{
		usleep(10);
		readData("oce", fds.oce);
		usleep(10);
		readData("val", fds.val);
		usleep(10);
		readData("eli", fds.eli);
	}
	else if (name == "val")
	{
		usleep(10);
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

	/*
		NO CREATION OF CLIENTS right away for
		* VAL
		* OCE
		because they are used in specific order in PASS, NICK, USER
		-> if you want to test only one specific test function except EXCEPT testPass(), testNick(), testUser()
		DECOMMENT the lines for creation of oce,val
	*/
//***********************************************************
	// // ------ Creation of client oce -----
	// sendMsg("oce", fds, "PASS ok\r\n");
	// sendMsg("oce", fds, "NICK oce\r\n");
	// sendMsg("oce", fds, "USER oce oce 127.0.0.1 oce\r\n");

	// // ------ Creation of client val -----
	// sendMsg("val", fds, "PASS ok\r\n");
	// sendMsg("val", fds, "NICK val\r\n");
	// sendMsg("val", fds, "USER val val 127.0.0.1 val\r\n");

	// ------ Creation of client eli -----
	std::cout << std::endl;
	sendMsg("eli", fds, "PASS ok\r\n");
	sendMsg("eli", fds, "NICK eli\r\n");
	sendMsg("eli", fds, "USER val val 127.0.0.1 val\r\n"); // on purpose that username is val (for test later on)
//*************************************************************************

//////////////////////////////////////////////
// LAUNCH TESTS

	/*
		-> NOTE: if you want to test only some specific test functions EXCEPT testPass(), testNick(), testUser()
		DECOMMENT the lines for creation of client oce, and client val above
		
		(MEANING: testPass(), testNick(), testUser() have to be tested TOGETHER, or removed TOGETHER
		If ALL 3 removed, decomment lines for creation of client oce and client val above)
	*/

	/*
		If ALL (or too many) tests are launched, redirect the results
		* FOR THIS CLIENT SIDE: c++ tests.cpp && ./a.out 127.0.0.1 6667 > res_client 2>&1
		* FOR SERVER SIDE: /remake.sh && valgrind  --leak-check=full --track-fds=yes --trace-children=yes ./ircserv 6667 ok > res_server 2>&1
		(you can remove valgrind if not needed)
		
		To read the results, execute this commands
		* more res_client
		* more res_server
	*/

	testPass(fds);
	testNick(fds);
	testUser(fds);
	
	testChannelNames(fds);
	testKey(fds);
	testInvite(fds);
	testChannelIsFullLogic(fds);
	testOtherJoinLogic(fds);
	testPrivmsg(fds);
	testKick(fds);
	testTopic(fds);
	testMode(fds);
	testPart(fds);
	testPing(fds);
	testChannelNameCaseInsensitive(fds);
	testPartialData(fds);
	testCrazyMsg(fds);
	testNicknamesChanged(fds);
	testQuit(fds);
	

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


void	testIsRegistered(fds fds)
{
	// send all those functions but the client is not even registered
	std::cout << "------------ test isRegistered" << std::endl;
	sendMsg("oce", fds, "INVITE mama #chan\r\n");
	sendMsg("val", fds, "JOIN #chan\r\n");
	sendMsg("oce", fds, "KICK #chan val\r\n");
	sendMsg("val", fds, "MODE #chan +k key\r\n");
	sendMsg("oce", fds, "PART #chan\r\n");
	sendMsg("val", fds, "PING ircserv\r\n");
	sendMsg("oce", fds, "PRIVMSG oce :hello!\r\n");
	sendMsg("val", fds, "QUIT\r\n");
	sendMsg("oce", fds, "TOPIC #chan\r\n");
	sendMsg("val", fds, "unknown command\r\n");
	sendMsg("oce", fds, "unknown command2\r\n");
}

void	testPass(fds fds)
{
	std::cout << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "PASS" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	testIsRegistered(fds);

	std::cout << "------------ ERR_NEEDMOREPARAMS" << std::endl;
	sendMsg("oce", fds, "PASS\r\n");
	sendMsg("oce", fds, "PASS \r\n");

	std::cout << "------------ ERR_PASSWDMISMATCH" << std::endl;
	sendMsg("oce", fds, "PASS l\r\n");

	std::cout << "------------ too many params" << std::endl;
	sendMsg("oce", fds, "PASS ok ok\r\n");

	std::cout << "> ------- BAD --> oce tries to do NICK, after all wrong password" << std::endl;
	sendMsg("oce", fds, "NICK oce\r\n");

	std::cout << "> ------- BAD --> oce tries to do USER, after all wrong password" << std::endl;
	sendMsg("oce", fds, "USER oce oce 127.0.0.1 oce\r\n");

	std::cout << std::endl;
	std::cout << "------------ good password" << std::endl;
	sendMsg("oce", fds, "PASS ok\r\n");

	std::cout << "> BAD --> oce tries to do USER, after password" << std::endl;
	sendMsg("oce", fds, "USER oce oce 127.0.0.1 oce\r\n");

	std::cout << std::endl;
	std::cout << "------------ BAD -> oce tries do PASS again" << std::endl;
	sendMsg("oce", fds, "PASS ok\r\n");
}

void	testNick(fds fds)
{	
	std::cout << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "NICK" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	testIsRegistered(fds);

	std::cout << std::endl;
	std::cout << "------------ GOOD nickname" << std::endl;
	sendMsg("oce", fds, "NICK oce\r\n");

		std::cout << std::endl;
	std::cout << "------------ too many params" << std::endl;
	sendMsg("oce", fds, "NICK oce oce\r\n");

	std::cout << std::endl;
	std::cout << "------------ BAD --> user before nick" << std::endl;
	sendMsg("val", fds, "USER val val 127.0.0.1 val\r\n");

	std::cout << std::endl;
	std::cout << "------------ BAD --> nick before pass" << std::endl;
	sendMsg("val", fds, "NICK val\r\n");

	std::cout << std::endl;
	std::cout << "------------ good pass" << std::endl;
	sendMsg("val", fds, "PASS ok\r\n");

	std::cout << "------------ good nick" << std::endl;
	sendMsg("val", fds, "NICK val\r\n");

	std::cout << std::endl;
	std::cout << "------------ ERR_NONICKNAMEGIVEN" << std::endl;
	sendMsg("val", fds, "NICK\r\n");

	std::cout << std::endl;
	std::cout << "------------ ERR_NICKNAMEINUSE" << std::endl;
	sendMsg("val", fds, "NICK oce\r\n");

	std::cout << std::endl;
	std::cout << "------------ ERR_ERRONEUSNICKNAME" << std::endl;

	/*
		nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
	*/
	
	// more than 9
	sendMsg("val", fds, "NICK a123456789\r\n"); 
	
	// starts with digit for first charac
	sendMsg("val", fds, "NICK 123456789\r\n");
	sendMsg("val", fds, "NICK 1mama\r\n");

	// starts with NO LETTER/NO SPECIAL for first charac
	sendMsg("val", fds, "NICK #val\r\n");
	sendMsg("val", fds, "NICK -val\r\n");
	sendMsg("val", fds, "NICK éal\r\n");

	// character is NOT LETTER, NOT SPECIAL, NOT DIGIT, NOT "-"
	sendMsg("val", fds, "NICK va&l\r\n");
	sendMsg("val", fds, "NICK valç\r\n");
	sendMsg("val", fds, "NICK v:lç\r\n");
	sendMsg("val", fds, "NICK v\rlç\r\n");
	sendMsg("val", fds, "NICK v\nlç\r\n");

	std::cout << std::endl;
	std::cout << "------------ GOOD nickname" << std::endl;

	// less or equal to 9 charac
	sendMsg("val", fds, "NICK a12345678\r\n");
	sendMsg("val", fds, "NICK baba-123\r\n"); // should pass
	sendMsg("val", fds, "NICK baba-1234\r\n"); // should pass


	/*
		nickname   =  ( letter / special ) *8( letter / digit / special / "-" )
	*/
	
	// a mix of everything (letter, special, digit, -)
	sendMsg("val", fds, "NICK [baba-12]\r\n");

	// with '-'
	sendMsg("val", fds, "NICK v-al\r\n"); // should pass
	sendMsg("val", fds, "NICK val-\r\n"); // should pass

	// with special for first charac
	sendMsg("val", fds, "NICK [val\r\n");
	sendMsg("val", fds, "NICK ]val\r\n");
	sendMsg("val", fds, "NICK \\val\r\n");
	sendMsg("val", fds, "NICK `val\r\n");
	sendMsg("val", fds, "NICK _val\r\n");
	sendMsg("val", fds, "NICK ^val\r\n");
	sendMsg("val", fds, "NICK {val\r\n");
	sendMsg("val", fds, "NICK |val\r\n");
	sendMsg("val", fds, "NICK }val\r\n");

	// with special NOT for first charac
	sendMsg("val", fds, "NICK v[val\r\n");
	sendMsg("val", fds, "NICK v]val\r\n");
	sendMsg("val", fds, "NICK v\\val\r\n");
	sendMsg("val", fds, "NICK v`val\r\n");
	sendMsg("val", fds, "NICK v_val\r\n");
	sendMsg("val", fds, "NICK v^val\r\n");
	sendMsg("val", fds, "NICK v{val\r\n");
	sendMsg("val", fds, "NICK v|val\r\n");
	sendMsg("val", fds, "NICK v}val\r\n");

	// 'val' nickname for the rest of the code
	sendMsg("val", fds, "NICK val\r\n");

}

void	testUser(fds fds)
{
	std::cout << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "USER" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	testIsRegistered(fds);

	/*
		user       =  1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )
                  ; any octet except NUL, CR, LF, " " and "@"
	*/
	std::cout << std::endl;
	std::cout << "------------ BAD user names" << std::endl;
	sendMsg("oce", fds, "USER oce\r oce 127.0.0.1 oce\r\n");
	sendMsg("oce", fds, "USER oce\n oce 127.0.0.1 oce\r\n");
	sendMsg("oce", fds, "USER oce@ oce 127.0.0.1 oce\r\n");
	sendMsg("oce", fds, "USER oc\re oce 127.0.0.1 oce\r\n");
	sendMsg("oce", fds, "USER oc\ne oce 127.0.0.1 oce\r\n");
	sendMsg("oce", fds, "USER oc@e oce 127.0.0.1 oce\r\n");
	sendMsg("oce", fds, "USER \roce oce 127.0.0.1 oce\r\n");
	sendMsg("oce", fds, "USER \noce oce 127.0.0.1 oce\r\n");
	sendMsg("oce", fds, "USER @oce oce 127.0.0.1 oce\r\n");

	std::cout << std::endl;
	std::cout << "------------ good" << std::endl;
	sendMsg("oce", fds, "USER oce oce 127.0.0.1 oce\r\n");
	sendMsg("val", fds, "USER val val 127.0.0.1 val\r\n");

	
	std::cout << std::endl;
	std::cout << "------------ ERR_ALREADYREGISTRED" << std::endl;
	sendMsg("oce", fds, "USER oce oce 127.0.0.1 oce\r\n");
	sendMsg("oce", fds, "PASS ok\r\n");

	std::cout << std::endl;
	std::cout << "------------ ERR_NEEDMOREPARAMS" << std::endl;
	sendMsg("oce", fds, "USER\r\n");
	sendMsg("oce", fds, "USER oce oce\r\n");
	sendMsg("oce", fds, "USER oce oce 127.0.0.1\r\n");
	
	std::cout << std::endl;
	std::cout << "------------ too many params" << std::endl;
	sendMsg("oce", fds, "USER oce oce 127.0.0.1 oce oce\r\n");


	std::cout << "------------ good prefix, but NICK ALREADY IN USE" << std::endl;
	sendMsg("oce", fds, ":oce NICK oce\r\n");

	std::cout << "------------ bad prefix" << std::endl;
	sendMsg("oce", fds, ":oce!oce@127.0.0.1NICK oce\r\n");
}

void testChannelNames(fds fds)
{
	std::cout << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "CHANNEL NAMES FORMAT " << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << std::endl;
	std::cout << "------------ BAD channel names" << std::endl;

	/*
			channel    =  ( "#" / "+" / ( "!" channelid ) / "&" ) chanstring
                [ ":" chanstring ]
			channelid  = 5( %x41-5A / digit )   ; 5( A-Z / 0-9 )
			chanstring =  %x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B
	 		chanstring =/ %x2D-39 / %x3B-FF
	            		; any octet except NUL, BELL, CR, LF, " ", "," and ":"`
	*/

	std::cout << "forbidden charac" << std::endl;
	sendMsg("eli", fds, "JOIN #ma\x07ma\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN #ma\ama\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN #ma\rma\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN #ma\nma\r\n"); // shout not pass
	sendMsg("eli", fds, "JOIN \x07ma\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN \ama\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN \rma\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN \nma\r\n"); // shout not pass
	sendMsg("eli", fds, "JOIN ma\x07\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN ma\a\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN ma\r\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN \\ma\n\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN \ra\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN \na\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN \r\r\n"); // should not pass
	sendMsg("eli", fds, "JOIN \n\r\n"); // should not pass
	
	// spaces forbidden
	sendMsg("eli", fds, "JOIN :# mama\r\n");
	sendMsg("eli", fds, "JOIN :#ma ma\r\n");
	sendMsg("eli", fds, "JOIN :#mama \r\n");


	// ':' forbidden
	sendMsg("eli", fds, "JOIN :#mama:\r\n");
	sendMsg("eli", fds, "JOIN #ma:ma\r\n");
	sendMsg("eli", fds, "JOIN :#:mama\r\n");

	// not tested, because our code considers #ma, and then ma
	// sendMsg("eli", fds, "JOIN :#ma,ma\r\n");


	// ---- bad 51 charac
	std::cout << "BAD -> 51 charac" << std::endl;
	sendMsg("eli", fds, "JOIN #01234567890123456789012345678901234567890123456789\r\n");
	

	// ---- bad channel id
	std::cout << "BAD -> bad channelid" << std::endl;
	sendMsg("eli", fds, "JOIN !A\r\n");
	sendMsg("eli", fds, "JOIN !AB\r\n");
	sendMsg("eli", fds, "JOIN !ABC\r\n");
	sendMsg("eli", fds, "JOIN !ABCD\r\n");
	sendMsg("eli", fds, "JOIN !A1CD\r\n");

	sendMsg("eli", fds, "JOIN !aBCDE\r\n");
	sendMsg("eli", fds, "JOIN !ABCDa\r\n");
	sendMsg("eli", fds, "JOIN !ABcDE\r\n");

	// ----- other bad channel name ( wrong first character)
	std::cout << "BAD -> bad channel name" << std::endl;
	sendMsg("eli", fds, "JOIN A\r\n");
	sendMsg("eli", fds, "JOIN -a\r\n");
	sendMsg("eli", fds, "JOIN ça\r\n");


	// ============== GOOD =============

	// --- good 50 charac
	std::cout << "GOOD: 50 charac" << std::endl;
	sendMsg("val", fds, "JOIN #0123456789012345678901234567890123456789012345678\r\n");

	// ---- good channel id
	/*
		channelid  = 5( %x41-5A / digit )   ; 5( A-Z / 0-9 )
	*/
	std::cout << "GOOD channelid" << std::endl;
	// with !
	sendMsg("oce", fds, "JOIN !ABCDE\r\n");
	sendMsg("oce", fds, "JOIN !ABCDEa\r\n");
	sendMsg("oce", fds, "JOIN !ABCDE-\r\n");
	sendMsg("oce", fds, "JOIN !1BCDE\r\n");
	sendMsg("oce", fds, "JOIN !AB9DE\r\n");
	sendMsg("oce", fds, "JOIN !ABCD0\r\n");
	sendMsg("oce", fds, "JOIN !ABCDEABCDE\r\n");

	std::cout << "GOOD channel names" << std::endl;
	// with #, +, &
	sendMsg("oce", fds, "JOIN #A\r\n");
	sendMsg("oce", fds, "JOIN +A\r\n");
	sendMsg("oce", fds, "JOIN &A\r\n");
	
	/* extended ascii
	 		chanstring =  %x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B
	 		chanstring =/ %x2D-39 / %x3B-FF
	            		; any octet except NUL, BELL, CR, LF, " ", "," and ":"`
	 -> so until FF here, extended ascii allowed! */
	sendMsg("oce", fds, "JOIN &ça\r\n");

}

void testKey(fds fds)
{
	// 	std::cout << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "KEY " << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	sendMsg("oce", fds, "JOIN #key\r\n");

	std::cout << "-------- BAD -> bad key" << std::endl;

	/*
		key        =  1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )
                  ; any 7-bit US_ASCII character,
                  ; except NUL, CR, LF, FF, h/v TABs, and " "
	*/

	// 24 letters
	sendMsg("oce", fds, "MODE #key +k 012345678901234567890123\r\n");

	// bad characters
	sendMsg("oce", fds, "MODE #key +k hello\rworld\r\n");
	sendMsg("oce", fds, "MODE #key +k hello\nworld\r\n");
	sendMsg("oce", fds, "MODE #key +k hello\fworld\r\n");
	sendMsg("oce", fds, "MODE #key +k hello\tworld\r\n");
	sendMsg("oce", fds, "MODE #key +k hello\vworld\r\n");

	sendMsg("oce", fds, "MODE #key +k \rworld\r\n");
	sendMsg("oce", fds, "MODE #key +k \nworld\r\n");
	sendMsg("oce", fds, "MODE #key +k \fworld\r\n");
	sendMsg("oce", fds, "MODE #key +k \tworld\r\n");
	sendMsg("oce", fds, "MODE #key +k \vworld\r\n");

	sendMsg("oce", fds, "MODE #key +k hello\r\r\n");
	sendMsg("oce", fds, "MODE #key +k hello\n\r\n");
	sendMsg("oce", fds, "MODE #key +k hello\f\r\n");
	sendMsg("oce", fds, "MODE #key +k hello\t\r\n");
	sendMsg("oce", fds, "MODE #key +k hello\v\r\n");
	
	// spaces
	sendMsg("oce", fds, "MODE #key +k :hello world\r\n");
	sendMsg("oce", fds, "MODE #key +k :helloworld \r\n");
	sendMsg("oce", fds, "MODE #key +k : helloworld\r\n");

	// only 7-bit US_ASCII character allowed
	sendMsg("oce", fds, "MODE #key +k ça\r\n");
	sendMsg("oce", fds, "MODE #key +k aé\r\n");
	

	std::cout << "-------- KEY LOGIC" << std::endl;
	// exactly 23 letters
	
	std::cout << "> oce sets a good key" << std::endl;
	sendMsg("oce", fds, "MODE #key +k 01234567890123456789012\r\n");

	std::cout << "> BAD --> val tries to join without a key" << std::endl;
	sendMsg("val", fds, "JOIN #key\r\n");

	std::cout << "> BAD --> val tries to join with a BAD key" << std::endl;
	sendMsg("val", fds, "JOIN #key oo\r\n");
	sendMsg("val", fds, "JOIN #key 0123456789012345678901\r\n");

	std::cout << "> GOOD -> val joins with GOOD key" << std::endl;
	sendMsg("val", fds, "JOIN #key 01234567890123456789012\r\n");
}

void testInvite(fds fds)
{
	std::cout << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "INVITE LOGIC " << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << "--------------- JOIN, INVITE" << std::endl;
	std::cout << "> oce & val joins #invite" << std::endl;
	sendMsg("oce", fds, "JOIN #invite\r\n");
	sendMsg("eli", fds, "JOIN #invite\r\n");

	std::cout << "> oce sets #invite invite-only" << std::endl;
	sendMsg("oce", fds, "MODE #invite +i\r\n");
	
	std::cout << "> BAD -> NOT OPERATOR, eli tries to set invite-only, but has NO OP" << std::endl;
	sendMsg("eli", fds, "MODE #invite +i\r\n");

	std::cout << "> BAD -> INVITE-ONLY, val tries to join an invite-only channel" << std::endl;
	sendMsg("val", fds, "JOIN #invite\r\n");

	// oce invites val
	std::cout << "> oce invites val" << std::endl;
	sendMsg("oce", fds, "INVITE val #invite\r\n");

	std::cout << "> now val can join" << std::endl;
	sendMsg("val", fds, "JOIN #invite\r\n");

	// val leaves and try to come again
	std::cout << "> BAD invite-only --> val leaves and tries to come again" << std::endl;
	sendMsg("val", fds, "PART #invite\r\n");

	std::cout << "> so val CANNOT join anymore" << std::endl;
	sendMsg("val", fds, "JOIN #invite\r\n");
	
	std::cout << "> check val is NOT in the channel" << std::endl;
	sendMsg("val", fds, "PART #invite\r\n");

	// eli invites val
	std::cout << "> BAD -> NOT OPERATOR eli tries to invites val" << std::endl;
	sendMsg("eli", fds, "INVITE val #invite\r\n");
	
	std::cout << "> GOOD -> oce gives op priv to eli" << std::endl;
	sendMsg("oce", fds, "MODE #invite +o eli\r\n");
	
	std::cout << "> -> then eli invites val" << std::endl;
	sendMsg("eli", fds, "INVITE val #invite\r\n");

	std::cout << "> -> then val rejoins the channel, and quits it again" << std::endl;
	sendMsg("val", fds, "JOIN #invite\r\n");
	sendMsg("val", fds, "PART #invite\r\n");

	std::cout << "> GOOD -> oce removes invite-only feature, and op priv from eli" << std::endl;
	sendMsg("oce", fds, "MODE #invite -io eli\r\n");
	
	/*
		When the channel has invite-only
		flag set, only channel operators may issue INVITE command.
	*/
	std::cout << "> GOOD -> #invite is NOT invite only anymore" << std::endl;
	std::cout << "> so eli can invite" << std::endl;
	sendMsg("eli", fds, "INVITE val #invite\r\n");

	std::cout << "--------------- INVITE COMMAND" << std::endl;
	
	// ERR_NEEDMOREPARAMS
	std::cout << "> ERR_NEEDMOREPARAMS" << std::endl;
	sendMsg("eli", fds, "INVITE\r\n");
	sendMsg("eli", fds, "INVITE #lo\r\n");

	std::cout << "> too many params" << std::endl;
	sendMsg("val", fds, "INVITE #lo oce oce\r\n");

	// ERR_NOSUCHNICK
	std::cout << "> ERR_NOSUCHNICK" << std::endl;
	sendMsg("oce", fds, "INVITE mama #invite\r\n");

	std::cout << "check val is NOT in the channel first" << std::endl;
	sendMsg("val", fds, "PART #invite\r\n");

	// ERR_NOTONCHANNEL
	std::cout << "> ERR_NOTONCHANNEL: val tries to invite, but he is not in the channel" << std::endl;
	sendMsg("val", fds, "INVITE eli #invite\r\n");

	// ERR_USERONCHANNEL
	std::cout << "> ERR_USERONCHANNEL" << std::endl;
	std::cout << "val joins the channel" << std::endl;
	sendMsg("val", fds, "JOIN #invite\r\n");

	std::cout << "oce invites val, but val is ALREADY in the channel" << std::endl;
	sendMsg("oce", fds, "INVITE val #invite\r\n");

	// ERR_CHANOPRIVSNEEDED 
	std::cout << "> ERR_CHANOPRIVSNEEDED" << std::endl;
	std::cout << "val quits the channel again" << std::endl;
	sendMsg("val", fds, "PART #invite\r\n");

	 /*
		When the channel has invite-only
		flag set, only channel operators may issue INVITE command.
	*/
	std::cout << "GOOD eli tries to invite him" << std::endl;
	std::cout << "and because channel is not invite-only" << std::endl;
	std::cout << "no op priv is needed" << std::endl;
	sendMsg("eli", fds, "INVITE val #invite\r\n");

	std::cout << "now oce sets channel as INVITE-ONLY" << std::endl;
	sendMsg("oce", fds, "MODE #invite +i\r\n");	

	std::cout << "BAD -> eli has NO OP PRIV, so he cannot invite anymore" << std::endl;
	sendMsg("eli", fds, "INVITE val #invite\r\n");

	
	/*
		// CHANNEL DOES NOT EXIST OR IS NOT VALID tests
		because RFC 2812:
			"There is no requirement that the
   			channel the target user is being invited to must exist or be a valid
   			channel."
	*/
	std::cout << "> GOOD --> RFC 2812: 'no requirement" << std::endl;
	std::cout << "that the channel the target user is being invited" << std::endl;
	std::cout << "must exist or be a valid channel'" << std::endl;
	sendMsg("oce", fds, "INVITE val #nochannel\r\n"); // does not exist
	sendMsg("eli", fds, "INVITE val #noc\nhannel\r\n"); // not a valid

	std::cout << "> -------- GOOD prefix" << std::endl;
	std::cout << "oce invites val" << std::endl;
	sendMsg("oce", fds, ":oce INVITE val #invite\r\n");
}

void	testChannelIsFullLogic(fds fds)
{
	
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "CHANNEL IS FULL logic " << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << "> oce joins #full" << std::endl;
	sendMsg("oce", fds, "JOIN #full\r\n");

	std::cout << "> val joins #full" << std::endl;
	sendMsg("val", fds, "JOIN #full\r\n");

	std::cout << "> val quits #full" << std::endl;
	sendMsg("val", fds, "PART #full\r\n");

	std::cout << "> oce sets user limit at 0 for #full" << std::endl;
	sendMsg("oce", fds, "MODE #full +l 0\r\n");

	std::cout << "> BAD-> USER LIMIT: val tries to join #full" << std::endl;
	sendMsg("val", fds, "JOIN #full\r\n");

	std::cout << "> oce sets user limit at 1 for #full" << std::endl;
	sendMsg("oce", fds, "MODE #full +l 1\r\n");

	std::cout << "> BAD-> USER LIMIT: val tries to join  #full" << std::endl;
	sendMsg("val", fds, "JOIN #full\r\n");

	std::cout << "> oce sets user limit at 2 for #full" << std::endl;
	sendMsg("oce", fds, "MODE #full +l 2\r\n");

	std::cout << "> val CAN join #full now" << std::endl;
	sendMsg("val", fds, "JOIN #full\r\n");
}

void	testOtherJoinLogic(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "other JOIN LOGIC" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << "------- PARAMS" << std::endl;

	std::cout << "> BAD: too many params" << std::endl;
	sendMsg("oce", fds, "JOIN #mama mama mama\r\n");

	std::cout << "> BAD: not enough params" << std::endl;
	sendMsg("oce", fds, "JOIN\r\n");

	std::cout << "------- TEST DUPLICATE JOIN" << std::endl;

	std::cout << "> GOOD -> val joins #duplicate" << std::endl;
	sendMsg("val", fds, "JOIN #duplicate\r\n");

	std::cout << "> GOOD -> val invites oce" << std::endl;
	sendMsg("val", fds, "INVITE oce #duplicate\r\n");

	std::cout << "> GOOD -> oce joins #duplicate" << std::endl;
	sendMsg("oce", fds, "JOIN #duplicate\r\n");

	std::cout << "> TEST DUPLICATE: this test is to make sure that" << std::endl;
	std::cout << "even though val sends 'JOIN #duplicate' TWICE!" << std::endl;
	std::cout << "When he leaves, he'll be really gone (like he should NOT have to send twice PART #duplicate)" << std::endl;

	std::cout << "> BAD --> val RESENDS a 'JOIN #duplicate' but he is ALREADY in it" << std::endl;
	sendMsg("val", fds, "JOIN #duplicate\r\n");

	std::cout << "> val sends a PART #duplicate" << std::endl;
	sendMsg("val", fds, "PART #duplicate\r\n");

	std::cout << "> CHECK that val is NOT in the channel anymore" << std::endl;
	sendMsg("val", fds, "INVITE oce #duplicate\r\n");

	std::cout << " ------- JOIN 0" << std::endl;
	
	std::cout << "> eli joins 3 channels" << std::endl;
	
	sendMsg("eli", fds, "JOIN #zero1\r\n");
	sendMsg("eli", fds, "JOIN #zero2\r\n");
	sendMsg("eli", fds, "JOIN #zero3\r\n");
	sendMsg("eli", fds, "JOIN #zero3 key\r\n"); // on purpose

	std::cout << "> oce joins channel #zero1" << std::endl;
	sendMsg("oce", fds, "JOIN #zero1\r\n");

	std::cout << "> check eli CAN do mode in all channels" << std::endl;
	sendMsg("eli", fds, "MODE #zero1 +k key\r\n");
	sendMsg("eli", fds, "MODE #zero2 +k key\r\n");

	std::cout << "> BAD too many params" << std::endl;
	sendMsg("eli", fds, "MODE #zero3 -k key2\r\n"); // on purpose
	sendMsg("eli", fds, "MODE #zero3 -k key\r\n"); // on purpose

	std::cout << "> BAD bad key" << std::endl;
	sendMsg("eli", fds, "MODE #zero1 -k key2\r\n"); // on purpose

	std::cout << "> GOOD remove key" << std::endl;
	sendMsg("eli", fds, "MODE #zero2 -k key\r\n"); // on purpose

	std::cout << "> eli leaves all channels (sends JOIN 0)" << std::endl;
	sendMsg("eli", fds, "JOIN 0\r\n");

	std::cout << "> CHECK eli is NOT in any channel anymore with MODE" << std::endl;
	sendMsg("eli", fds, "MODE #zero1 +k key\r\n");
	sendMsg("eli", fds, "MODE #zero2 +k key\r\n");
	sendMsg("eli", fds, "MODE #zero3 +k key\r\n");
	
	std::cout << "> CHECK eli is NOT in any channel anymore with PART" << std::endl;
	sendMsg("eli", fds, "PART #zero1\r\n");
	sendMsg("eli", fds, "PART #zero2\r\n");
	sendMsg("eli", fds, "PART #zero3\r\n");


	std::cout << " ------- JOIN #foo,#bar fubar,foobar  " << std::endl;
	
	std::cout << "> oce joins #foo, #bar" << std::endl;
	sendMsg("oce", fds, "JOIN #foo,#bar\r\n");

	std::cout << "> oce sets keys fubar,foobar" << std::endl;
	sendMsg("oce", fds, "MODE #foo +k fubar\r\n");
	sendMsg("oce", fds, "MODE #bar +k foobar\r\n");

	std::cout << "> CHECK val is NOT in any of the channels #foo,#bar" << std::endl;
	sendMsg("val", fds, "PART #foo,#bar\r\n");
	
	std::cout << "> BAD --> val tries to join #foo,#bar with bad keys" << std::endl;
	sendMsg("val", fds, "JOIN #foo,#bar bad,bad\r\n");
	sendMsg("val", fds, "PART #foo,#bar\r\n");

	std::cout << "> BAD --> space between params" << std::endl;
	sendMsg("val", fds, "JOIN #foo,#bar #foo, #bar\r\n");// space between the keys
	sendMsg("val", fds, "PART #foo,#bar\r\n");

	std::cout << "> val CAN join #foo,#bar with good keys" << std::endl;
	sendMsg("val", fds, "JOIN #foo,#bar fubar,foobar\r\n");
	sendMsg("val", fds, "PART #foo,#bar\r\n");

	std::cout << "> BAD -> too many params" << std::endl;
	sendMsg("val", fds, "JOIN #foo,#bar bad,bad,bad\r\n");
	sendMsg("val", fds, "JOIN #foo,#bar bad,bad,bad ok\r\n");

	std::cout << "> BAD & GOOD -> eli tries to join #foo,#bar (one BAD key & one GOOD key)" << std::endl;
	sendMsg("eli", fds, "JOIN #foo,#bar BAD,foobar\r\n");
	
	std::cout << "> GOOD & BAD -> eli can only leave #bar" << std::endl;
	sendMsg("eli", fds, "PART #foo,#bar\r\n");

	std::cout << "> GOOD & BAD -> eli tries to join #foo,#bar (one GOOD key & one BAD key)" << std::endl;
	sendMsg("eli", fds, "JOIN #foo,#bar fubar,BAD\r\n");

	std::cout << "> GOOD & BAD -> eli can only leave #foo" << std::endl;
	sendMsg("eli", fds, "PART #foo,#bar\r\n");

	std::cout << " --------------- JOIN #foo,&bar fubar" << std::endl;
	
	std::cout << "> oce joins &bar" << std::endl;
	sendMsg("oce", fds, "JOIN &bar\r\n");

	std::cout << "> CHECK eli is NOT in #foo, and &bar" << std::endl;
	sendMsg("eli", fds, "PART #foo,&bar\r\n");

	std::cout << "> BAD --> eli tries to join #foo and &bar (one BAD key for #foo)" << std::endl;
	sendMsg("eli", fds, "JOIN #foo,&bar BAD\r\n");

	std::cout << "> CHECK eli is NOT in #foo, and &bar" << std::endl;
	sendMsg("eli", fds, "PART #foo,&bar\r\n");

	std::cout << "> eli CAN join #foo and &bar (GOOD KEY)" << std::endl;
	sendMsg("eli", fds, "JOIN #foo,&bar fubar \r\n");

	std::cout << "> eli leaves #foo and &bar " << std::endl;
	sendMsg("eli", fds, "PART #foo,&bar\r\n");

	std::cout << " --------------- good prefix" << std::endl;
	sendMsg("oce", fds, ":oce JOIN #pref\r\n");

	std::cout << " --------------- good prefix but UNKNOWN COMMAND" << std::endl;
	sendMsg("oce", fds, ":oce JOIN#pref\r\n");

	std::cout << " --------------- bad prefix" << std::endl;
	sendMsg("oce", fds, ": oce JOIN #pref\r\n");
	sendMsg("oce", fds, ":\noce JOIN #pref\r\n");
}

void testPrivmsg(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "PRIVMSG" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << "--------- ERR_NORECIPIENT " << std::endl;
	sendMsg("oce", fds, "PRIVMSG\r\n");

	std::cout << "--------- ERR_NOTEXTTOSEND " << std::endl;
	sendMsg("eli", fds, "PRIVMSG oce\r\n");

	std::cout << "--------- too many params " << std::endl;
	sendMsg("val", fds, "PRIVMSG oce oce oce\r\n");

	std::cout << "--------- ERR_CANNOTSENDTOCHAN " << std::endl;

	std::cout << "> oce joins #cannot" << std::endl;
	sendMsg("oce", fds, "JOIN #cannot\r\n");

	std::cout << "> BAD --> eli tries to send a msg to #cannot" << std::endl;
	std::cout << "but IS NOT in the channel" << std::endl;
	sendMsg("eli", fds, "PRIVMSG #cannot :hello!\r\n");

	std::cout << "---------  ERR_NOSUCHNICK " << std::endl;
	sendMsg("val", fds, "PRIVMSG unknown :hello\r\n");
	sendMsg("val", fds, "PRIVMSG oce1 :hello\r\n");
	sendMsg("val", fds, "PRIVMSG 1oce :hello\r\n");

	/*
		WE DID:
			sendMsg("oce", fds, "PASS ok\r\n");
			sendMsg("oce", fds, "NICK oce\r\n");
			sendMsg("oce", fds, "USER oce oce 127.0.0.1 oce\r\n");

			sendMsg("val", fds, "PASS ok\r\n");
			sendMsg("val", fds, "NICK val\r\n");
			sendMsg("val", fds, "USER val val 127.0.0.1 val\r\n");

			std::cout << std::endl;
			std::cout << "------------ creation of a new client ELI" << std::endl;
			sendMsg("eli", fds, "PASS ok\r\n");
			sendMsg("eli", fds, "NICK eli\r\n");
			sendMsg("eli", fds, "USER val val 127.0.0.1 val\r\n");
		SO NOW:
		1) nick!user@hostname
		
			oce -> oce!oce@127.0.0.1
			val -> val!val@127.0.0.1
			eli -> eli!val@127.0.0.1 => it was done on purpose to do the ERR_TOOMANYTARGETS
			
		2) user@server
		
			oce -> oce@ircserv
			val -> val@ircserv
			eli -> val@ircserv => it was done on purpose to do the ERR_TOOMANYTARGETS

		3) user%hostname@server
			oce -> oce%127.0.0.1@ircserv
			val -> val%127.0.0.1@ircserv
			eli -> val%127.0.0.1@ircserv => it was done on purpose to do the ERR_TOOMANYTARGETS

		4) user%hostname
			oce -> oce%127.0.0.1
			val -> val%127.0.0.1
			eli -> val%127.0.0.1 => it was done on purpose to do the ERR_TOOMANYTARGETS
	*/


	std::cout << "--------- ERR_TOOMANYTARGETS " << std::endl;
	// val and eli can reproduce that error -> they have the same username

	/*
		for this example :
		PRIVMSG jto@tolsun.oulu.fi :Hello !
                                   ; Command to send a message to a user
                                   on server tolsun.oulu.fi with
                                   username of "jto".
		for us, for user@server
		we have 
			val -> val@ircserv
			eli -> val@ircserv => it was done on purpose to do the ERR_TOOMANYTARGETS
		so this one : sendMsg("eli", fds, "PRIVMSG val@ircserv :hello\r\n"); 
		should send a TOOMANYTARGETS error
	*/
	sendMsg("eli", fds, "PRIVMSG val@ircserv :hello\r\n"); // supposed to have error too many targets
	sendMsg("eli", fds, "PRIVMSG val%127.0.0.1@ircserv :hello\r\n");
	sendMsg("eli", fds, "PRIVMSG val%127.0.0.1 :hello\r\n");

	std::cout << "--------- GOOD: eli to oce" << std::endl;
	sendMsg("eli", fds, "PRIVMSG oce!oce@127.0.0.1 :OLA\r\n");

	/*
		RFC 2812

		PRIVMSG jto@tolsun.oulu.fi :Hello !
                                   ; Command to send a message to a user
                                   on server tolsun.oulu.fi with
                                   username of "jto"
	*/
	sendMsg("eli", fds, "PRIVMSG oce@ircserv :HELLO\r\n"); // it's supposed to work
	sendMsg("eli", fds, "PRIVMSG oce%127.0.0.1@ircserv :BONJOUR\r\n");
	sendMsg("eli", fds, "PRIVMSG oce%127.0.0.1 :CIAO\r\n");

	std::cout << "--------- GOOD: val to eli" << std::endl;
	sendMsg("val", fds, "PRIVMSG eli!val@127.0.0.1 :OLA\r\n");
	
	// 
	std::cout << "--------- BAD does not exist" << std::endl;
	/*
			eli -> eli!val@127.0.0.1 => it was done on purpose to do the ERR_TOOMANYTARGETS
			eli -> val@ircserv => it was done on purpose to do the ERR_TOOMANYTARGETS
			eli -> val%127.0.0.1@ircserv => it was done on purpose to do the ERR_TOOMANYTARGETS
			eli -> val%127.0.0.1 => it was done on purpose to do the ERR_TOOMANYTARGETS
	*/
	sendMsg("val", fds, "PRIVMSG eli!eli@127.0.0.1 :OLA\r\n");
	sendMsg("val", fds, "PRIVMSG eli@ircserv :HELLO\r\n");
	sendMsg("val", fds, "PRIVMSG eli%127.0.0.1@ircserv :BONJOUR\r\n");
	sendMsg("val", fds, "PRIVMSG eli%127.0.0.1 :CIAO\r\n");
	

	std::cout << "--------- BAD nick!user@hostname" << std::endl;
	sendMsg("val", fds, "PRIVMSG eli!eli@127.0.0.1 :OLA\r\n"); // test because it's eli!val@127.0.0.1
	sendMsg("val", fds, "PRIVMSG oc1e!oce@127.0.0.1 :OLA\r\n");
	sendMsg("val", fds, "PRIVMSG oce!oc1e@127.0.0.1 :OLA\r\n");
	sendMsg("val", fds, "PRIVMSG oce!oce@127.10.0.1 :OLA\r\n");

	sendMsg("eli", fds, "PRIVMSG oce@oce!127.0.0.1 :OLA\r\n"); // Should not work !
	sendMsg("val", fds, "PRIVMSG @oceoce!127.0.0.1 :OLA\r\n");
	sendMsg("val", fds, "PRIVMSG !oce@oce127.0.0.1 :OLA\r\n");
	sendMsg("val", fds, "PRIVMSG oce@oce127.0.0.1! :OLA\r\n");
	sendMsg("val", fds, "PRIVMSG oce!oce127.0.0.1@ :OLA\r\n");
	sendMsg("val", fds, "PRIVMSG oce!@oce127.0.0.1 :OLA\r\n");
	sendMsg("val", fds, "PRIVMSG oce@oce!127.!0.0.1 :OLA\r\n");
	sendMsg("val", fds, "PRIVMSG oce@oce!127.0.@0.1 :OLA\r\n");


	std::cout << "--------- BAD user@server" << std::endl;
	sendMsg("eli", fds, "PRIVMSG o1ce@ircserv :HELLO\r\n");
	sendMsg("eli", fds, "PRIVMSG oce@irc1serv :HELLO\r\n");

	// sendMsg("eli", fds, "PRIVMSG oce@ircserv :HELLO\r\n"); // the good one
	sendMsg("eli", fds, "PRIVMSG oce@ircserv@ :HELLO\r\n");
	sendMsg("eli", fds, "PRIVMSG @oceircserv :HELLO\r\n");
	sendMsg("eli", fds, "PRIVMSG oceircserv@ :HELLO\r\n");
	 

	std::cout << "--------- BAD user%hostname@server" << std::endl;
	sendMsg("val", fds, "PRIVMSG oc1e%127.0.0.1@ircserv :BONJOUR\r\n");
	sendMsg("val", fds, "PRIVMSG oce%127.0.10.1@ircserv :BONJOUR\r\n");
	sendMsg("val", fds, "PRIVMSG oce%127.0.0.1@irc1serv :BONJOUR\r\n");

	// sendMsg("eli", fds, "PRIVMSG oce%127.0.0.1@ircserv :BONJOUR\r\n") // the good one
	sendMsg("val", fds, "PRIVMSG oce@127.0.0.1%ircserv :BONJOUR\r\n");
	sendMsg("val", fds, "PRIVMSG @oce%127.0.0.1ircserv :BONJOUR\r\n");
	sendMsg("val", fds, "PRIVMSG %oce@127.0.0.1ircserv :BONJOUR\r\n");
	sendMsg("val", fds, "PRIVMSG oce127.0.0.1@ircserv% :BONJOUR\r\n");
	sendMsg("val", fds, "PRIVMSG oce%127.0.0.1ircserv@ :BONJOUR\r\n");
	sendMsg("val", fds, "PRIVMSG oce%127.0.0.1@irc@serv :BONJOUR\r\n");
	sendMsg("val", fds, "PRIVMSG oce%127.%0.0.1@ircserv :BONJOUR\r\n");
	sendMsg("val", fds, "PRIVMSG o@ce127.0.0.1ircs%erv :BONJOUR\r\n");
	sendMsg("val", fds, "PRIVMSG o@%ce127.0.0.1ircserv :BONJOUR\r\n");


	std::cout << "--------- BAD user%hostname" << std::endl;
	sendMsg("eli", fds, "PRIVMSG oce%127.10.0.1 :CIAO\r\n");
	sendMsg("eli", fds, "PRIVMSG oce1%127.0.0.1 :CIAO\r\n");

	// sendMsg("eli", fds, "PRIVMSG oce%127.0.0.1 :CIAO\r\n"); // the good one
	sendMsg("eli", fds, "PRIVMSG oce127.0.0.1% :CIAO\r\n");
	sendMsg("eli", fds, "PRIVMSG %oce127.0.0.1 :CIAO\r\n");
	sendMsg("eli", fds, "PRIVMSG oce%127.0.0.1% :CIAO\r\n");


	// 1) nick!user@hostname

	// 2) user@server

	// 3) user%hostname@server

	// 4) user%hostname

	std::cout << "--------- other BAD" << std::endl;
	sendMsg("eli", fds, "PRIVMSG !oce127.%0.0@.1 :CIAO\r\n"); // !%@
	sendMsg("eli", fds, "PRIVMSG oc!e12@7.%0.0.1 :CIAO\r\n"); // !@%
	sendMsg("eli", fds, "PRIVMSG oc!e127.%0.0.1@ :CIAO\r\n"); // !%@
	
	sendMsg("eli", fds, "PRIVMSG oc!e12@7.%0.0.1 :CIAO\r\n"); // !@%
	sendMsg("eli", fds, "PRIVMSG oc%e12@7.!0.0.1 :CIAO\r\n"); // %@!
	sendMsg("eli", fds, "PRIVMSG oce12@7%.!0.0.1 :CIAO\r\n"); // %@!

	sendMsg("eli", fds, "PRIVMSG oc%e127.!0.0.1 :CIAO\r\n"); // %!
	sendMsg("eli", fds, "PRIVMSG oc!e127.%0.0.1 :CIAO\r\n"); // !%

	std::cout << "--------- PRIVMSG nickname: val to oce" << std::endl;
	sendMsg("val", fds, "PRIVMSG oce :it's val!\r\n");

	std::cout << "--------- PRIVMSG channel" << std::endl;
	
	std::cout << "> oce in #msg1, #msg2" << std::endl;
	sendMsg("oce", fds, "JOIN #msg1,#msg2\r\n");

	std::cout << "> val in #msg2, #msg3" << std::endl;
	sendMsg("val", fds, "JOIN #msg2,#msg3\r\n");

	std::cout << "> eli in #msg1, #msg2, #msg3" << std::endl;
	sendMsg("eli", fds, "JOIN #msg1,#msg2,#msg3\r\n");

	std::cout << "> eli sends to #msg1 (eli, oce)" << std::endl;
	sendMsg("eli", fds, "PRIVMSG #msg1 :yo msg1!\r\n");

	std::cout << "> eli sends to #msg2 (eli, oce, val)" << std::endl;
	sendMsg("eli", fds, "PRIVMSG #msg2 :msg2!\r\n"); 

	std::cout << "> eli sends to #msg3 (eli, val)" << std::endl;
	sendMsg("eli", fds, "PRIVMSG #msg3 :msg3 is here?\r\n");

	std::cout << "--------- good prefix" << std::endl;
	sendMsg("val", fds, ":val PRIVMSG oce :it's val!\r\n");

	std::cout << "> eli sends to #msg3 (eli, val)" << std::endl;
	sendMsg("eli", fds, ":eli PRIVMSG #msg3 :msg3 is here?\r\n");

	std::cout << "--------- bad prefix" << std::endl;
	sendMsg("eli", fds, ":eli\r PRIVMSG #msg3 :msg3 is here?\r\n");
}

void testKick(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "KICK" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << "--------- ERR_NEEDMOREPARAMS " << std::endl;
	sendMsg("oce", fds, "KICK\r\n");

	std::cout << "--------- too many params " << std::endl;
	sendMsg("val", fds, "KICK oce oce oce oce\r\n");

	std::cout << "--------- ERR_NEEDMOREPARAMS " << std::endl;
	std::cout << "> 'as many channel parameters as there are user parameters'" << std::endl;
	sendMsg("eli", fds, "KICK #hello,#hola oce\r\n");
	sendMsg("eli", fds, "KICK #hello,#hola oce,oce,oce\r\n");

	std::cout << "--------- ERR_NOSUCHCHANNEL" << std::endl;
	sendMsg("oce", fds, "KICK #hello oce\r\n");
	sendMsg("oce", fds, "KICK #hello,#hola oce,oce\r\n");

	std::cout << "--------- ERR_NOTONCHANNEL" << std::endl;
	std::cout << "> val joins #kick1" << std::endl;
	sendMsg("val", fds, "JOIN #kick1\r\n");

	std::cout << "> eli joins #kick2" << std::endl;
	sendMsg("eli", fds, "JOIN #kick2\r\n");
	
	std::cout << "> check oce is NOT in #kick1,#kick2" << std::endl;
	sendMsg("oce", fds, "PART #kick1,#kick2\r\n");
	
	std::cout << "> BAD -> oce tries to kick" << std::endl;
	sendMsg("oce", fds, "KICK #kick1,#kick2 val,eli\r\n");
	sendMsg("oce", fds, "KICK #kick1 val\r\n");
	
	std::cout << "--------- ERR_NOTINCHANNEL" << std::endl;
	std::cout << "> check oce is NOT in #kick1,#kick2" << std::endl;
	sendMsg("oce", fds, "PART #kick1,#kick2\r\n");
	
	std::cout << "> BAD -> val tries to kick oce" << std::endl;
	sendMsg("val", fds, "KICK #kick1 oce\r\n");

	std::cout << "> check eli is NOT in #kick1" << std::endl;
	sendMsg("eli", fds, "PART #kick1\r\n");

	std::cout << "> BAD -> val tries to kick oce, and eli" << std::endl;
	sendMsg("val", fds, "KICK #kick1,#kick1 oce,eli\r\n");

	std::cout << "--------- ERR_CHANOPRIVSNEEDED" << std::endl;
	std::cout << "> oce finally joins #kick1,#kick2" << std::endl;
	sendMsg("oce", fds, "JOIN #kick1,#kick2\r\n");

	std::cout << "> BAD -> oce tries to kick" << std::endl;
	sendMsg("oce", fds, "KICK #kick1,#kick2 val,eli\r\n");

	std::cout << "> BAD -> oce tries to kick" << std::endl;
	sendMsg("oce", fds, "KICK #kick1 val\r\n");

	std::cout << "--------- GOOD" << std::endl;
	std::cout << "> val gives op priv to oce" << std::endl;
	sendMsg("val", fds, "MODE #kick1 +o oce\r\n");

	std::cout << "> eli gives op priv to oce" << std::endl;
	sendMsg("eli", fds, "MODE #kick2 +o oce\r\n");
	
	std::cout << "> GOOD -> oce kicks val and eli" << std::endl;
	sendMsg("oce", fds, "KICK #kick1,#kick2 val,eli\r\n");

	std::cout << "> check val and eli are both kicked" << std::endl;
	sendMsg("val", fds, "PART #kick1\r\n");
	sendMsg("eli", fds, "PART #kick2\r\n");

	std::cout << "> val rejoins #kick1" << std::endl;
	sendMsg("val", fds, "JOIN #kick1\r\n");

	std::cout << "> oce rekicks val" << std::endl;
	sendMsg("oce", fds, "KICK #kick1 val\r\n");

	std::cout << "> check val is NOT in the channel anymore" << std::endl;
	sendMsg("val", fds, "PART #kick1\r\n");

	std::cout << "--------- this time, one channel with multiple users" << std::endl;
	std::cout << "> val joins #kick3,#kick4" << std::endl;
	sendMsg("val", fds, "JOIN #kick3,#kick4\r\n");

	std::cout << "> eli joins #kick4,#kick5" << std::endl;
	sendMsg("eli", fds, "JOIN #kick4,#kick5\r\n");

	std::cout << "> check oce is NOT in #kick3,#kick4,#kick5" << std::endl;
	sendMsg("oce", fds, "PART #kick3,#kick4,#kick5\r\n");

	std::cout << "> BAD -> oce tries to kick BUT NOTONCHANNEL" << std::endl;
	sendMsg("oce", fds, "KICK #kick3 val,eli\r\n");

	std::cout << "> check eli is NOT in #kick3" << std::endl;
	sendMsg("eli", fds, "PART #kick3\r\n");

	std::cout << "> BAD -> val tries to kick oce,eli BUT BOTH NOTINCHANNEL" << std::endl;
	sendMsg("val", fds, "KICK #kick3 oce,eli\r\n");

	std::cout << "> check oce is NOT in #kick3" << std::endl;
	sendMsg("oce", fds, "PART #kick3\r\n");

	std::cout << "> GOOD & BAD -> val succeeds to kick eli, BUT NOT OCE" << std::endl;
	std::cout << "eli is in #kick4 BUT NOT oce" << std::endl;
	sendMsg("val", fds, "KICK #kick4 eli,oce\r\n");

	std::cout << "> check oce is NOT in #kick4" << std::endl;
	sendMsg("oce", fds, "PART #kick4\r\n");

	std::cout << "> check eli is NOT in #kick4 anymore" << std::endl;
	sendMsg("eli", fds, "PART #kick4\r\n");

	std::cout << "> eli rejoins #kick4" << std::endl;
	sendMsg("eli", fds, "JOIN #kick4\r\n");

	std::cout << "> oce joins #kick4" << std::endl;
	sendMsg("oce", fds, "JOIN #kick4\r\n");

	std::cout << "> BAD -> oce tries to kick BUT NO OP PRIV" << std::endl;
	sendMsg("oce", fds, "KICK #kick4 val,eli\r\n");

	std::cout << "> GOOD & GOOD -> val succeeds to kick eli, and oce" << std::endl;
	sendMsg("val", fds, "KICK #kick4 eli,oce\r\n");
	
	std::cout << "--------- GOOD: test comments" << std::endl;
	std::cout << "> eli & val joins #kick1" << std::endl;
	sendMsg("val", fds, "JOIN #kick1\r\n");
	sendMsg("eli", fds, "JOIN #kick1\r\n");

	std::cout << "> oce kicks val with NO comment" << std::endl;
	sendMsg("oce", fds, "KICK #kick1 val\r\n");

	std::cout << "> oce kicks eli WITH COMMENT" << std::endl;
	sendMsg("oce", fds, "KICK #kick1 eli :bad behavior\r\n");

	std::cout << "> eli rejoins #kick1" << std::endl;
	sendMsg("eli", fds, "JOIN #kick1\r\n");
	
	std::cout << "--------- GOOD: prefix" << std::endl;
	std::cout << ">oce kicks eli WITH COMMENT" << std::endl;
	sendMsg("oce", fds, ":oce KICK #kick1 eli :bad behavior\r\n");
}

void testTopic(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "TOPIC LOGIC" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << "--------- ERR_NEEDMOREPARAMS " << std::endl;
	sendMsg("oce", fds, "TOPIC\r\n");

	std::cout << "--------- too many params " << std::endl;
	sendMsg("val", fds, "TOPIC #topic oce oce\r\n");

	std::cout << "---------  ERR_NOTONCHANNEL" << std::endl;
	sendMsg("eli", fds, "TOPIC #topic\r\n");
	sendMsg("eli", fds, "TOPIC #topic newTopic\r\n");

	std::cout << "--------- ERR_CHANOPRIVSNEEDED" << std::endl;
	
	std::cout << "> oce joins #topic, so oce is OP" << std::endl;
	sendMsg("oce", fds, "JOIN #topic\r\n");
	
	std::cout << "> val joins #topic, so NO OP PRIV (2nd to enter the channel)" << std::endl;
	sendMsg("val", fds, "JOIN #topic\r\n");

	std::cout << "> GOOD --> val tries TOPIC, and it works, topic is not only for op right now" << std::endl;
	sendMsg("val", fds, "TOPIC #topic\r\n");

	std::cout << "> oce will set topic ONLY FOR OP" << std::endl;
	sendMsg("oce", fds, "MODE #topic +t\r\n");

	std::cout << "> BAD --> val tries TOPIC, but this time TOPIC is for OP" << std::endl;
	sendMsg("val", fds, "TOPIC #topic\r\n");
	
	std::cout << "> oce gives val OP PRIV" << std::endl;
	sendMsg("oce", fds, "MODE #topic +o val\r\n");

	std::cout << "> GOOD --> val tries TOPIC, and it works, val has OP PRIV" << std::endl;
	sendMsg("val", fds, "TOPIC #topic\r\n");

	std::cout << "--------- RPL_NOTOPIC" << std::endl;

	std::cout << "> GOOD RPL_NOTOPIC --> val tries TOPIC" << std::endl;
	sendMsg("val", fds, "TOPIC #topic\r\n");

	std::cout << "--------- RPL_TOPIC" << std::endl;
	std::cout << "> GOOD RPL_TOPIC --> val sets a topic" << std::endl;
	sendMsg("val", fds, "TOPIC #topic :New topic\r\n");

	std::cout << "> GOOD RPL_TOPIC --> oce views the topic" << std::endl;
	sendMsg("oce", fds, "TOPIC #topic\r\n");

	std::cout << "--------- CLEAR THE TOPIC" << std::endl;
	std::cout << "> GOOD RPL_NOTOPIC clear the topic --> val clears the topic" << std::endl;
	sendMsg("val", fds, "TOPIC #topic :\r\n");

	std::cout << "> GOOD RPL_NOTOPIC --> oce views the topic (which was cleared)" << std::endl;
	sendMsg("oce", fds, "TOPIC #topic\r\n");

	std::cout << "--------- GOOD: prefix" << std::endl;
	sendMsg("oce", fds, ":oce TOPIC #topic\r\n");

	std::cout << "--------- BAD: prefix" << std::endl;
	sendMsg("oce", fds, ":oce! TOPIC #topic\r\n");
	sendMsg("oce", fds, ":oc1eTOPIC #topic\r\n");
	sendMsg("oce", fds, ":oc1e!o TOPIC #topic\r\n");
}

void testMode(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "MODE" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << "> oce joins #mode" << std::endl;
	sendMsg("oce", fds, "JOIN #mode\r\n");

	std::cout << "--------- ERR_NEEDMOREPARAMS " << std::endl;
	sendMsg("oce", fds, "MODE \r\n");
	sendMsg("oce", fds, "MODE #mode +l\r\n");
	sendMsg("oce", fds, "MODE #mode +k\r\n");
	sendMsg("oce", fds, "MODE #mode +o\r\n");
	sendMsg("oce", fds, "MODE #mode -o\r\n");
	sendMsg("oce", fds, "MODE #mode -p+kl-o key\r\n"); 
	sendMsg("oce", fds, "MODE #mode -klopo key\r\n");

	std::cout << "--------- too many params " << std::endl;
	sendMsg("oce", fds, "MODE #mode +i val\r\n");
	sendMsg("oce", fds, "MODE #mode -i val\r\n");
	sendMsg("oce", fds, "MODE #mode +t val\r\n");
	sendMsg("oce", fds, "MODE #mode -t val\r\n");
	sendMsg("oce", fds, "MODE #mode +k key key\r\n");
	sendMsg("oce", fds, "MODE #mode +o val val\r\n");
	sendMsg("oce", fds, "MODE #mode -o val val\r\n");
	sendMsg("oce", fds, "MODE #mode +l 20 20\r\n");
	sendMsg("oce", fds, "MODE #mode -l 20\r\n");
	sendMsg("oce", fds, "MODE #mode -kwlw key 0 ma\r\n");
	sendMsg("oce", fds, "MODE #mode +kwlw key 0 ma\r\n");
	sendMsg("oce", fds, "MODE #mode +kwlw-o key 0 ma pa\r\n"); 
	sendMsg("oce", fds, "MODE #mode +kwlw+o key 0 ma pa\r\n"); 
	
	std::cout << "> GOOD -> check you can join (meaning that no k above was executed)" << std::endl;
	sendMsg("val", fds, "JOIN #mode\r\n");

	std::cout << "--------- ERR_CHANOPRIVSNEEDED" << std::endl;
	sendMsg("val", fds, "MODE #mode\r\n");
	
	std::cout << "--------- RPL_CHANNELMODEIS" << std::endl;
	sendMsg("oce", fds, "MODE #mode\r\n");

	std::cout << "--------- MODE +/-i" << std::endl;
	std::cout << "> oce sets #mode INVITE-ONLY" << std::endl;
	sendMsg("oce", fds, "MODE #mode +i\r\n");

	std::cout << "> BAD -> eli tries to join #mode, which is invite-only" << std::endl;
	sendMsg("eli", fds, "JOIN #mode\r\n");
	
	std::cout << "> oce removes invite-only feature" << std::endl;
	sendMsg("oce", fds, "MODE #mode -i\r\n");

	std::cout << "> GOOD -> eli joins #mode" << std::endl;
	sendMsg("eli", fds, "JOIN #mode\r\n");

	std::cout << "--------- MODE +/-t" << std::endl;
	std::cout << std::endl;
	
	std::cout << "> oce sets a topic 'your topic!" << std::endl;
	sendMsg("oce", fds, "TOPIC #mode :your topic!\r\n");

	std::cout << "> GOOD -> val views topic" << std::endl;
	sendMsg("val", fds, "TOPIC #mode\r\n");

	std::cout << "> oce allows topic only for OP" << std::endl;
	sendMsg("oce", fds, "MODE #mode +t\r\n");

	std::cout << "> BAD -> val tries to view topic but has NO OP PRIV" << std::endl;
	sendMsg("val", fds, "TOPIC #mode\r\n");

	std::cout << "> oce removes 'topic only for OP'" << std::endl;
	sendMsg("oce", fds, "MODE #mode -t\r\n");

	std::cout << "> GOOD -> val views the topic" << std::endl;
	sendMsg("val", fds, "TOPIC #mode\r\n");

	std::cout << "--------- MODE +/-k" << std::endl;
	std::cout << std::endl;

	std::cout << "> eli leaves #mode" << std::endl;
	sendMsg("eli", fds, "PART #mode\r\n");

	std::cout << "> check eli is NOT in #mode" << std::endl;
	sendMsg("eli", fds, "PART #mode\r\n");
	
	std::cout << "> GOOD -> check eli can join #mode WITHOUT any key" << std::endl;
	sendMsg("eli", fds, "JOIN #mode\r\n");
	sendMsg("eli", fds, "PART #mode\r\n");

	std::cout << "> oce sets a key 'secretKey'" << std::endl;
	sendMsg("oce", fds, "MODE #mode +k secretKey\r\n");

	std::cout << "> BAD (ERR_KEYSET) --> oce tries to set a key AGAIN" << std::endl;
	sendMsg("oce", fds, "MODE #mode +k secretKey2\r\n"); // done on purpose for test below

	std::cout << "> BAD -> eli joins without a key" << std::endl;
	sendMsg("eli", fds, "JOIN #mode\r\n");

	std::cout << "> BAD -> eli joins with bad key" << std::endl;
	sendMsg("eli", fds, "JOIN #mode secretKey2\r\n"); // this one is obviously wrong, because it was used for ERR_KEYSET test

	std::cout << "> GOOD -> eli joins with good key" << std::endl;
	sendMsg("eli", fds, "JOIN #mode secretKey\r\n");

	std::cout << "> check eli was in #mode" << std::endl;
	sendMsg("eli", fds, "PART #mode\r\n");

	std::cout << "> oce removes key" << std::endl;
	sendMsg("oce", fds, "MODE #mode -k secretKey\r\n");

	std::cout << "> GOOD -> eli joins without a key" << std::endl;
	sendMsg("eli", fds, "JOIN #mode\r\n");

	std::cout << "> just to check that now that key was removed, oce can sets new one, and remove it" << std::endl;
	sendMsg("oce", fds, "MODE #mode +k newKey\r\n");
	sendMsg("oce", fds, "MODE #mode -k newKey\r\n");

	std::cout << "--------- MODE +/-o" << std::endl;
	std::cout << std::endl;

	std::cout << "> BAD (ERR_CHANOPRIVSNEEDED) -> val tries MODE #mode, but has no NO OP PRIV " << std::endl;
	sendMsg("val", fds, "MODE #mode\r\n");

	std::cout << "> oce gives him op priv" << std::endl;
	sendMsg("oce", fds, "MODE #mode +o val\r\n");
	
	std::cout << "> GOOD -> val does MODE #mode" << std::endl;
	sendMsg("val", fds, "MODE #mode\r\n");

	std::cout << "> oce removes op priv from val" << std::endl;
	sendMsg("oce", fds, "MODE #mode -o val\r\n");

	std::cout << "> BAD (ERR_CHANOPRIVSNEEDED) -> val tries MODE #mode, but has no NO OP PRIV " << std::endl;
	sendMsg("val", fds, "MODE #mode\r\n");

	std::cout << "> val leaves #mode" << std::endl;
	sendMsg("val", fds, "PART #mode\r\n");

	std::cout << "> BAD (ERR_USERNOTINCHANNEL) -> oce tries to set/remove op priv to people NOT IN CHANNEL" << std::endl;
	sendMsg("oce", fds, "MODE #mode -o val\r\n");
	sendMsg("oce", fds, "MODE #mode +o val\r\n");
	sendMsg("oce", fds, "MODE #mode -o unknown\r\n");
	sendMsg("oce", fds, "MODE #mode +o unknown\r\n");

	std::cout << "--------- MODE +/-l" << std::endl;
	std::cout << std::endl;
	
	sendMsg("val", fds, "JOIN #mode\r\n");
	std::cout << "> oce, val, eli are all in #mode" << std::endl;
	std::cout << std::endl;

	std::cout << "> eli leaves #mode" << std::endl;
	sendMsg("eli", fds, "PART #mode\r\n");

	// user limit 0
	std::cout << "> oce sets user limit to 0" << std::endl;
	sendMsg("oce", fds, "MODE #mode +l 0\r\n");

	std::cout << "> BAD -> eli tries to rejoin #mode" << std::endl;
	sendMsg("eli", fds, "JOIN #mode\r\n");

	std::cout << "> check eli is NOT in channel #mode" << std::endl;
	sendMsg("eli", fds, "PART #mode\r\n");
	
	// user limit 1
	std::cout << "> oce sets user limit to 1" << std::endl;
	sendMsg("oce", fds, "MODE #mode +l 1\r\n");

	std::cout << "> BAD -> eli tries to rejoin #mode" << std::endl;
	sendMsg("eli", fds, "JOIN #mode\r\n");

	std::cout << "> check eli is NOT in channel #mode" << std::endl;
	sendMsg("eli", fds, "PART #mode\r\n");
	
	// user limit 2
	std::cout << "> oce sets user limit to 2" << std::endl;
	sendMsg("oce", fds, "MODE #mode +l 2\r\n");

	std::cout << "> BAD -> eli tries to rejoin #mode" << std::endl;
	sendMsg("eli", fds, "JOIN #mode\r\n");

	std::cout << "> check eli is NOT in channel #mode" << std::endl;
	sendMsg("eli", fds, "PART #mode\r\n");

	// user limit 3
	std::cout << "> oce sets user limit to 3" << std::endl;
	sendMsg("oce", fds, "MODE #mode +l 3\r\n");

	std::cout << "> GOOD -> eli rejoins #mode" << std::endl;
	sendMsg("eli", fds, "JOIN #mode\r\n");
	
	std::cout << "> check eli is IN channel #mode" << std::endl;
	sendMsg("eli", fds, "PART #mode\r\n");	

	// reset limit to 0, to test -l
	std::cout << "> oce sets user limit to 0 AGAIN" << std::endl;
	sendMsg("oce", fds, "MODE #mode +l 0\r\n");

	std::cout << "> BAD -> eli tries to rejoin #mode" << std::endl;
	sendMsg("eli", fds, "JOIN #mode\r\n");
	
	std::cout << "> check eli is NOT in channel #mode" << std::endl;
	sendMsg("eli", fds, "PART #mode\r\n");

	std::cout << "> oce REMOVES user limit" << std::endl;
	sendMsg("oce", fds, "MODE #mode -l\r\n");

	std::cout << "> GOOD -> eli rejoins #mode" << std::endl;
	sendMsg("eli", fds, "JOIN #mode\r\n");


	std::cout << ">>> BAD (NOT NUMERIC) oce sets user limit something not numeric" << std::endl;
	sendMsg("oce", fds, "MODE #mode +l elmkd\r\n");
	sendMsg("oce", fds, "MODE #mode +l 1elmkd\r\n");
	sendMsg("oce", fds, "MODE #mode +l 12332l\r\n");
	sendMsg("oce", fds, "MODE #mode +l 12332o\r\n");

	std::cout << "--------- unkwown mode" << std::endl;
	sendMsg("oce", fds, "MODE #mode +m\r\n");
	sendMsg("oce", fds, "MODE #mode -s\r\n");
	sendMsg("oce", fds, "MODE #mode -w+w-z\r\n");
	sendMsg("oce", fds, "MODE #mode mama\r\n");

	std::cout << "check no mode was set" << std::endl;
	sendMsg("oce", fds, "MODE #mode\r\n");	


	std::cout << "-------- limit of 3 modes" << std::endl;
	
	std::cout << "test that '+o eli' is not executed" << std::endl;
	sendMsg("oce", fds, "MODE #mode +tkolo key val 0 eli\r\n"); // eli is not executed

	std::cout << "test that 'o val' has been executed" << std::endl;
	std::cout << "BUT NOT 'o eli'" << std::endl;
	sendMsg("val", fds, "MODE #mode\r\n");

	std::cout << "BAD --> test that 'o eli' has NOT been executed" << std::endl;
	sendMsg("eli", fds, "MODE #mode\r\n");

	std::cout << "val leaves #mode" << std::endl;
	sendMsg("val", fds, "PART #mode\r\n");

	std::cout << "BAD --> test that 'k key' has been executed" << std::endl;
	std::cout << "so you need a key to join" << std::endl;
	sendMsg("val", fds, "JOIN #mode\r\n");

	std::cout << "test that 'l 0' has been executed" << std::endl;
	std::cout << "-> the key will be correct" << std::endl;
	std::cout << " BAD --> but you still CANNOT join because of USER LIMIT 0" << std::endl;
	sendMsg("val", fds, "JOIN #mode key\r\n");

	std::cout << "-------- MODE +kw+l-v yourKey 0" << std::endl;
	std::cout << "> reset everything " << std::endl;

	
	std::cout << "-------- BAD -> need more params key missing with -k, so o will not have a param" << std::endl;
	sendMsg("oce", fds, "MODE #mode -itkool val eli\r\n"); // test with minus -k key

	std::cout << "--------GOOD reset everything" << std::endl;
	sendMsg("oce", fds, "MODE #mode -itkool key val eli\r\n"); // test with minus -k key

	// in case request above was not working
	// sendMsg("oce", fds, "MODE #mode -i\r\n");
	// sendMsg("oce", fds, "MODE #mode -t\r\n");
	// sendMsg("oce", fds, "MODE #mode -k\r\n");
	// sendMsg("oce", fds, "MODE #mode -o val\r\n");
	// sendMsg("oce", fds, "MODE #mode -o eli\r\n");
	// sendMsg("oce", fds, "MODE #mode -l\r\n");


	std::cout << ">>> send MODE +kw+l-v yourKey 0" << std::endl;
	std::cout << "- bad w" << std::endl;
	std::cout << "- bad v" << std::endl;
	std::cout << "- k : yourKey" << std::endl;
	std::cout << "- l : 0" << std::endl;
	sendMsg("oce", fds, "MODE #mode +kw+l-v yourKey 0\r\n");

	std::cout << "BAD --> test that 'k key' has been executed" << std::endl;
	std::cout << "-> you need a key to join" << std::endl;
	sendMsg("val", fds, "JOIN #mode\r\n");

	std::cout << "test that 'l 0' has been executed" << std::endl;
	std::cout << "-> the key will be correct" << std::endl;
	std::cout << " BAD -->  but you still CANNOT join because of USER LIMIT 0" << std::endl;
	sendMsg("val", fds, "JOIN #mode yourKey\r\n");


	std::cout << "-------- Mode +k key +l 0 -l -k +o unknown +t +i +z" << std::endl;
	std::cout << std::endl;
	std::cout << "> reset everything " << std::endl;
	sendMsg("eli", fds, "PART #mode\r\n");
	sendMsg("val", fds, "PART #mode\r\n");
	sendMsg("oce", fds, "MODE #mode -i\r\n");
	sendMsg("oce", fds, "MODE #mode -t\r\n");
	sendMsg("oce", fds, "MODE #mode -k yourKey\r\n");
	sendMsg("oce", fds, "MODE #mode -l\r\n");

	std::cout << ">>> send MODE +k key +l 0 -l -k +o unknown +t +i +z" << std::endl;
	std::cout << "- bad z" << std::endl;
	std::cout << "- unknown NOT EXECUTED limit reached (only t & i will be executed afterwards, and z unknown command)" << std::endl;
	std::cout << "- no user limit" << std::endl;
	std::cout << "- topic for OPERATOR" << std::endl;
	std::cout << "- #mode INVITE_ONLY" << std::endl;
	sendMsg("oce", fds, "MODE #mode +k key +l 0 -l -k key +o unknown +t +i +z\r\n");

	std::cout << "> BAD -> val tries to join but it's INVITE ONLY" << std::endl;
	sendMsg("val", fds, "JOIN #mode\r\n");

	std::cout << "> oce removes invite only feature" << std::endl;
	sendMsg("oce", fds, "MODE #mode -i\r\n");

	std::cout << "> GOOD -> val joins #mode" << std::endl;
	sendMsg("val", fds, "JOIN #mode\r\n");

	std::cout << "> BAD -> val tries TOPIC, but it's only for OP" << std::endl;
	sendMsg("val", fds, "TOPIC #mode\r\n");

	std::cout << "-------- Mode +k ma\nma +l 12 -l -k ma\nma -t -i -z" << std::endl;
	std::cout << "just add a limit l to test something later" << std::endl;
	sendMsg("oce", fds, "MODE #mode +l 2\r\n");
	
	std::cout << "- invalid key format" << std::endl;
	std::cout << "- NO user limit of 2 anymore" << std::endl;
	std::cout << "- topic NOT for op" << std::endl;
	std::cout << "- bad z" << std::endl;
	sendMsg("oce", fds, "MODE #mode +k ma\nma +l 12 -l -t -z\r\n");

	std::cout << "> GOOD -> val does TOPIC, but it's NOT only for OP anymore" << std::endl;
	sendMsg("val", fds, "TOPIC #mode\r\n");

	std::cout << "- BAD too many params" << std::endl;
	sendMsg("oce", fds, "MODE #mode +k ma\nma +l 12 -l -k ma\nma\r\n");

	std::cout << "-------- good prefix" << std::endl;
	sendMsg("oce", fds, ":oce MODE #mode +t\r\n");
}

void	testPart(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "PART" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << "> oce joins #part" << std::endl;
	sendMsg("oce", fds, "JOIN #part\r\n");

	std::cout << "> val joins #part2" << std::endl;
	sendMsg("val", fds, "JOIN #part2\r\n");

	std::cout << "--------- ERR_NEEDMOREPARAMS " << std::endl;
	sendMsg("eli", fds, "PART\r\n");

	std::cout << "--------- too many params " << std::endl;
	sendMsg("oce", fds, "PART oce oce oce\r\n");

	std::cout << "--------- ERR_NOSUCHCHANNEL " << std::endl;
	sendMsg("val", fds, "PART #nosuch\r\n");
	sendMsg("val", fds, "PART #nosuch :nosuch\r\n");
	sendMsg("val", fds, "PART #nosuch,#nosuch2,#nosuch3 :nosuch\r\n");

	std::cout << "--------- ERR_NOTONCHANNEL " << std::endl;
	std::cout << "> only val is in #part2" << std::endl;
	sendMsg("oce", fds, "PART #part2\r\n");

	std::cout << "> only oce is in #part" << std::endl;
	sendMsg("val", fds, "PART #part\r\n");

	std::cout << "--------- PART #part,#nosuch,#part2,#part3" << std::endl;
	sendMsg("eli", fds, "JOIN #part,#part3,&part4,+part5,+part6,&part7\r\n");
	
	std::cout << ">>> eli sends PART #part,#nosuch,#part2,#part3" << std::endl;
	std::cout << "- he is in #part with oce" << std::endl;
	std::cout << "- #nosuch does not exist" << std::endl;
	std::cout << "- he is NOT in #part2" << std::endl;
	std::cout << "- he is in #part3" << std::endl;
	sendMsg("eli", fds, "PART #part,#nosuch,#part2,#part3\r\n");

	std::cout << "--------- PART with or without a comment" << std::endl;
	std::cout << "> eli leaves WITHOUT a comment" << std::endl;
	sendMsg("eli", fds, "PART &part4\r\n");

	std::cout << "> eli leaves WITH a comment" << std::endl;
	sendMsg("eli", fds, "PART +part5 :no interest anymore\r\n");

	std::cout << "--------- BAD prefix" << std::endl;
	sendMsg("eli", fds, ":eli1 PART +part6\r\n");
	sendMsg("eli", fds, ":eli!v1al PART +part6\r\n");
	sendMsg("eli", fds, ":-eli!val PART +part6\r\n");
	
	std::cout << "--------- GOOD prefix" << std::endl;
	std::cout << "> eli leaves +part6" << std::endl;
	sendMsg("eli", fds, ":eli PART +part6\r\n");

	std::cout << "> eli leaves &part7" << std::endl;
	sendMsg("eli", fds, ":eli PART &part7 :no interest anymore\r\n");
}

void	testPing(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "PING" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << "--------- ERR_NEEDMOREPARAMS " << std::endl;
	sendMsg("oce", fds, "PING\r\n");

	std::cout << "--------- too many params" << std::endl;
	sendMsg("eli", fds, "PING ircserv ok\r\n");

	std::cout << "--------- ERR_NOSUCHSERVER " << std::endl;
	sendMsg("val", fds, "PING klfj\r\n");
	sendMsg("val", fds, "PING ircserv1\r\n");

	std::cout << "--------- ERR_NOSUCHSERVER " << std::endl;
	sendMsg("oce", fds, "PING ircserv\r\n");
}

void	testChannelNameCaseInsensitive(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "CHANNEL NAMES ARE CASE INSENSITIVE" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << "--------- test JOIN #abcde" << std::endl;
	std::cout << "> everyone is notified" << std::endl;
	sendMsg("oce", fds, "JOIN #ABCDE\r\n");
	sendMsg("eli", fds, "JOIN #AbCdE\r\n");
	sendMsg("val", fds, "JOIN #aBcDe\r\n");

	std::cout << "> mode (everyone notified)" << std::endl;
	sendMsg("oce", fds, "MODE #aBcDe +l 30\r\n");
	
	std::cout << "> val & oce receive msg" << std::endl;
	sendMsg("eli", fds, "PRIVMSG #abCDe :hola los amigos!\r\n");

	std::cout << "> val sets topic, and oce views it "<< std::endl;
	sendMsg("val", fds, "TOPIC #AbCDe :okok\r\n");
	sendMsg("oce", fds, "TOPIC #AbCde\r\n");
	
	std::cout << "> val is ALREADY in the channel "<< std::endl;
	sendMsg("oce", fds, "INVITE val #ABCDe\r\n");

	std::cout << "> oce kicks val (everyone is notified) "<< std::endl;
	sendMsg("oce", fds, "KICK #ABCDE val\r\n");
	
	std::cout << "> oce and eli leave the same channel" << std::endl;
	std::cout << "(eli is notified when oce leaves)" << std::endl;
	sendMsg("oce", fds, "PART #abcde\r\n");
	sendMsg("eli", fds, "PART #abcdE\r\n");

	std::cout << "--------- test JOIN !ABCDEFG" << std::endl;
	std::cout << "> everyone is notified" << std::endl;
	sendMsg("oce", fds, "JOIN !ABCDEFG\r\n");
	sendMsg("val", fds, "JOIN !ABCDEfG\r\n");
	sendMsg("eli", fds, "JOIN !ABCDEFg\r\n");

	std::cout << "--------- test PART !ABCDEFG" << std::endl;
	std::cout << "> everyone is notified" << std::endl;
	sendMsg("oce", fds, "PART !ABCDEfG\r\n");
	sendMsg("val", fds, "PART !ABCDEFG\r\n");
	sendMsg("eli", fds, "PART !ABCDEFg\r\n");
}

void		testPartialData(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "PARTIAL DATA" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << "--------- test with one person OCE" << std::endl;

	sendMsg("oce", fds, "JO");
	usleep(100);
	sendMsg("oce", fds, "IN");
	sendMsg("oce", fds, " ");
	sendMsg("oce", fds, "#be");
	usleep(1000);
	
	std::cout << "> CHECK -> oce joins 'bestChannelEver'" << std::endl;
	sendMsg("oce", fds, "stChannelEver\r\nTOPIC #bestChannelEver");
	
	sendMsg("oce", fds, " :NEW TOPIC!");
	
	std::cout << "> CHECK -> oce views the topic 'NEW TOPIC of #bestChannelEver" << std::endl;
	sendMsg("oce", fds, "\r\n");


	std::cout << ">reset by oce leaving #bestChannelEver" << std::endl;
	sendMsg("oce", fds, "PART #bestChannelEver\r\n");

	std::cout << "--------- test with one person VAL" << std::endl;

	sendMsg("val", fds, "JO");
	usleep(100);
	sendMsg("val", fds, "IN ");
	sendMsg("val", fds, "#bestChannelEver");
	
	std::cout << "> CHECK -> val joins 'bestChannelEver'" << std::endl;
	sendMsg("val", fds, "\r\n");

	std::cout << ">reset by val leaving #bestChannelEver" << std::endl;
	sendMsg("val", fds, "PART #bestChannelEver\r\n");


	std::cout << "--------- test with one person ELI" << std::endl;

	sendMsg("eli", fds, "JO");
	usleep(100);
	sendMsg("eli", fds, "IN ");
	std::cout << "> CHECK -> eli joins 'bestChannelEver'" << std::endl;
	sendMsg("eli", fds, "#bestChannelEver\r\n");
	
	sendMsg("eli", fds, "\r\n");

	std::cout << ">reset by eli leaving #bestChannelEver" << std::endl;
	sendMsg("eli", fds, "PART #bestChannelEver\r\n");

	std::cout << "--------- test with 3 people" << std::endl;

	sendMsg("oce", fds, "JO");
	usleep(100);
	sendMsg("oce", fds, "IN");
	sendMsg("val", fds, "JO");
	sendMsg("oce", fds, " ");
	sendMsg("oce", fds, "#be");

	sendMsg("eli", fds, "JO");
	usleep(100);
	sendMsg("eli", fds, "IN ");
	usleep(1000);
	
	sendMsg("val", fds, "IN ");
	sendMsg("val", fds, "#bestChannelEver");

	std::cout << "> CHECK -> oce joins 'bestChannelEver'" << std::endl;
	sendMsg("oce", fds, "stChannelEver\r\nTOPIC #bestChannelEver");

	std::cout << "> CHECK -> eli joins 'bestChannelEver'" << std::endl;
	sendMsg("eli", fds, "#bestChannelEver\r\n");

	std::cout << "> CHECK -> val joins 'bestChannelEver'" << std::endl;
	sendMsg("val", fds, "\r\n");

	
	sendMsg("oce", fds, " :NEW TOPIC!");
	
	std::cout << "> CHECK -> oce views the topic 'NEW TOPIC of #bestChannelEver" << std::endl;
	sendMsg("oce", fds, "\r\n");

	std::cout << "> eli leaves #bestChannelEver" << std::endl;
	sendMsg("eli", fds, "PART #bestChannelEver\r\n");

	std::cout << "> val leaves #bestChannelEver" << std::endl;
	sendMsg("val", fds, "PART #bestChannelEver\r\n");
	
	std::cout << "> oce leaves #bestChannelEver" << std::endl;
	sendMsg("oce", fds, "PART #bestChannelEver\r\n");
}

void		testCrazyMsg(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "CRAZY MESSAGES" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;

	std::cout << ">-------------------------" << std::endl;
	sendMsg("eli", fds, "\r\n\r\n\r\nJO");
	usleep(100);
	sendMsg("eli", fds, "IN ");
	
	std::cout << "> CHECK -> eli joins 'bestChannel'" << std::endl;
	sendMsg("eli", fds, "#bestChannel\r\n");
	
	std::cout << ">-------------------------" << std::endl;
	std::cout << "> nothing to do" << std::endl;
	sendMsg("eli", fds, "\r\n");

	std::cout << ">-------------------------" << std::endl;
	std::cout << "> unknown command \n\a\v\n" << std::endl;
	sendMsg("eli", fds, "\n\a\v\n\r\n");

	std::cout << ">-------------------------" << std::endl;	
	sendMsg("eli", fds, "\r\r\n\r\nJOIN #weird0"); 
	
	std::cout << "> CHECK -> eli joins 'weird0'" << std::endl;
	sendMsg("eli", fds, "\r\n\n\r\nJOIN\a#weird1\n");

	std::cout << "> CHECK -> eli joins 'weird2'" << std::endl;
	sendMsg("eli", fds, "\r\r\n\r\nJOIN #weird2\r\n\a\r\n");
}

void		testNicknamesChanged(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "NICKNAME CHANGES" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	
	std::cout << "they all join same channel #newnick" << std::endl;
	sendMsg("eli", fds, "JOIN #newnick\r\n");
	sendMsg("oce", fds, "JOIN #newnick\r\n");
	sendMsg("val", fds, "JOIN #newnick\r\n");

	std::cout << "they all join same channel #newnick2" << std::endl;
	sendMsg("eli", fds, "JOIN #newnick2\r\n");
	sendMsg("oce", fds, "JOIN #newnick2\r\n");
	sendMsg("val", fds, "JOIN #newnick2\r\n");

	std::cout << "eli is now eli2\r\n" << std::endl;
	sendMsg("eli", fds, "NICK eli2\r\n");

	//------------------------ eli (now eli2) leaves all channel

	std::cout << "------------------ eli (now eli2) leaves all channel" << std::endl;


	std::cout << "GOOD - eli (now eli2) leaves channel #newnick" << std::endl;
	std::cout << "-> IT SHOULD WORK even though eli changed his nickname" << std::endl;
	std::cout << "->everyone is notified" << std::endl;
	sendMsg("eli", fds, "PART #newnick\r\n");

	std::cout << "GOOD - eli (now eli2) leaves channel #newnick2" << std::endl;
	std::cout << "-> IT SHOULD WORK even though eli changed his nickname" << std::endl;
	std::cout << "->everyone is notified" << std::endl;
	sendMsg("eli", fds, "PART #newnick2\r\n");

	//------------------------ #newnick

	std::cout << "------------------ #newnick" << std::endl;

	
	std::cout << "oce is now oce2" << std::endl;
	sendMsg("oce", fds, "NICK oce2\r\n");

	std::cout << "BAD , NOT OPERATOR- val tries to kick oce(now oce2)" << std::endl;
	sendMsg("val", fds, "KICK #newnick oce2\r\n");

	std::cout << "GOOD -> oce (now oce2) gives privilege to val" << std::endl;
	std::cout << "- eli left so now oce is operator" << std::endl;
	sendMsg("oce", fds, "MODE #newnick +o val\r\n");

	std::cout << "BAD , NICKNAME NOT PRESENT -> val tries to kick using 'oce' nickname BUT NOW oce is oce2" << std::endl;
	sendMsg("val", fds, "KICK #newnick oce\r\n");

	std::cout << "GOOD -> val kicks using good nickname 'oce2'" << std::endl;
	sendMsg("val", fds, "KICK #newnick oce2\r\n");

	//------------------------ #newnick2

	std::cout << "------------------ #newnick2" << std::endl;


	std::cout << "val is now val2" << std::endl;
	sendMsg("val", fds, "NICK val2\r\n");

	std::cout << "GOOD -> oce (now oce2) makes #newnick2 INVITE ONLY" << std::endl;
	std::cout << "- eli left so now oce is operator\r\n" << std::endl;
	sendMsg("oce", fds, "MODE #newnick2 +i\r\n");

	std::cout << "BAD , NOSUCHNICK --> oce tries to invite val in the channel with 'val' (but val is now val2)" << std::endl;
	sendMsg("oce", fds, "INVITE val #newnick2\r\n");

	std::cout << "BAD  - ALREADY IN CHANNEL - oce tries to invite val2" << std::endl;
	sendMsg("val", fds, "INVITE val2 #newnick2\r\n");

	std::cout << "GOOD -> val2 leaves channel #newnick2" << std::endl;
	std::cout << "only oce is notified (eli already left before)" << std::endl;
	sendMsg("val", fds, "PART #newnick2\r\n");

	std::cout << "BAD - the channel is INVITE ONLY" << std::endl;
	sendMsg("val", fds, "JOIN #newnick2\r\n");
	
	std::cout << "GOOD - oce invites val2" << std::endl;
	sendMsg("oce", fds, "INVITE val2 #newnick2\r\n");

	std::cout << "val2 is now 'val' AGAIN" << std::endl;
	sendMsg("val", fds, "NICK val\r\n");

	std::cout << "GOOD -> val should be able to join #newnick2 even though he was invited with old nickname" << std::endl;
	sendMsg("val", fds, "JOIN #newnick2\r\n");

	std::cout << "reset all other nicknames" << std::endl;
	sendMsg("eli", fds, "NICK eli\r\n");
	sendMsg("oce", fds, "NICK oce\r\n");
}


void		testQuit(fds fds)
{
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
	std::cout << "QUIT" << std::endl;
	std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;


	std::cout << "--------- too many params" << std::endl;
	sendMsg("eli", fds, "QUIT ok ok\r\n");

	std::cout << "--------- RESET EVERYTHING" << std::endl;
	std::cout << std::endl;
	
	std::cout << "> reset all channels -> everyone leaves their respective channels" << std::endl;
	sendMsg("oce", fds, "JOIN 0\r\n");
	sendMsg("val", fds, "JOIN 0\r\n");
	sendMsg("eli", fds, "JOIN 0\r\n");

	std::cout << "> they all join #quit" << std::endl;
	sendMsg("oce", fds, "JOIN #quit\r\n");
	sendMsg("val", fds, "JOIN #quit\r\n");
	sendMsg("eli", fds, "JOIN #quit\r\n");

	std::cout << "> they all join #quit2" << std::endl;
	sendMsg("oce", fds, "JOIN #quit2\r\n");
	sendMsg("val", fds, "JOIN #quit2\r\n");
	sendMsg("eli", fds, "JOIN #quit2\r\n");

	std::cout << "> check only oce is operator" << std::endl;
	sendMsg("oce", fds, "MODE #quit\r\n");
	sendMsg("val", fds, "MODE #quit\r\n");
	sendMsg("eli", fds, "MODE #quit\r\n");

	std::cout << "--------- oce quits first WITHOUT A COMMENT" << std::endl;
	
	std::cout << "> CHECK only one message is sent to everyone!!" << std::endl;
	sendMsg("oce", fds, "QUIT\r\n");

	std::cout << "> check val is now operator" << std::endl;
	sendMsg("val", fds, "MODE #quit\r\n");
	sendMsg("eli", fds, "MODE #quit\r\n");

	std::cout << "> BAD --> val tries to send a msg to oce" << std::endl;
	sendMsg("val", fds, "PRIVMSG oce :are you here? :(\r\n");

	std::cout << "> val sends msg to channel, ONLY ELI RECEIVES" << std::endl;
	sendMsg("val", fds, "PRIVMSG #quit :who's there? :(\r\n");

	std::cout << "----- BAD --> try to make oce sends something to the server" << std::endl;
	sendMsg("oce", fds, "JOIN #quit\r\n");

	std::cout << "--------- val quits WITH A COMMENT" << std::endl;
	sendMsg("val", fds, "QUIT :too tired today!\r\n");

	std::cout << "> check eli is now operator" << std::endl;
	sendMsg("eli", fds, "MODE #quit\r\n");

	std::cout << "> BAD --> check val is not in the server anymore" << std::endl;
	sendMsg("val", fds, "MODE #quit\r\n");

	std::cout << "--------- finally, eli quits" << std::endl;
	sendMsg("eli", fds, "QUIT :bye bye!\r\n");

	sendMsg("eli", fds, "MODE #quit\r\n");
}