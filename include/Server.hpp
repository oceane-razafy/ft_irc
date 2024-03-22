/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmourtia <vmourtia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 20:40:06 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/20 13:02:27 by vmourtia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# define BUFFER_SIZE 1024
# include "Configuration.hpp"
# include "Parser.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Utils.hpp"
# include "Colors.hpp"
# include "Logger.hpp"
# include <string>
# include <vector>
# include <map>
# include <poll.h>
# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <cstdio>
# include <poll.h>
# include <algorithm>
# include <unistd.h>
# include <cerrno>
# include <csignal>

// DEBUG
# include <cstring> // memset
# include <fstream> // istringstream

class Parser;

class Server
{
	public:
										Server(const std::string &port, const std::string &password);
		
		// Functions called when server is closing
										~Server(void);
		void							cleanUp(void);
		
		// Displaying errors	
		void							throwRuntimeError(const std::string &prefixErrMsg) const;

		// Functions to handle signals
		void							setUpSignalHandlers(void);
		static void						signalHandler(int sigNum);
		static bool						hasShutDown(void);
		
		// Initialize and run the server
		void							init(void);
		void 							run(void);
		
		// Functions needed when server is running
		void							handleNewClient(void);
		Client							*getClient(int fd);
		void							handleEvents(pollfd clientPollfd);
		void							handleClientRequest(Client *client);
		void							removeClient(Client *client);
		void							handleNewMsgToClient(const std::string &msg, Client *client);
		void							sendAllPendingMsgsToClient(Client *client);

		// Functions needed for other classes
		Client							*getClient(const std::string &nickname);
		std::map<int, Client>			getClients(void) const;
		Configuration					getConfig(void) const;
		Channel							*getChannel(std::string channelName) const;
		void							addNewChannel(std::string channelName, const std::string &creatorName);
		void							broadcastMsgToAllChannelMembers(Channel *channel, const std::string &msg);
		void							broadcastMsgToAllMembers(const std::string &msg);

		std::vector<std::string>		getNicknameFromUsername(const std::string &username) const;

	private:
		Configuration 					_config;
		Parser							*_parser;

		int								_fd;
		struct sockaddr_in				_address;
		std::vector<pollfd>				_pollfds;

		// Map with all the client of ther server
		// -> Client fd, Client object
		std::map<int, Client> 			_clients;

		// Map with all the channels of the server
		// -> Name of the channel, Channel object
		std::map<std::string, Channel>	_channels;
		

		// when signal will be handled
		static bool						_shutDown;
};

#endif