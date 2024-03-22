/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmourtia <vmourtia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 20:39:01 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/21 13:50:20 by vmourtia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::_shutDown = false;

/* ==================================================================================
    * Constructor
================================================================================== */

Server::Server(const std::string &port, const std::string &password)
	: _config(port, password),
	_pollfds(),
	_clients(),
	_channels()
{
	// Pass the server object to the parser
	// because parser will need server object to update channels map if needed
	_parser = new Parser(this);
	if (_parser == NULL)
		throw std::runtime_error("ERROR: Parser allocation failed");
	
	// Try to initialize server
	try
	{
		setUpSignalHandlers();
		init();
	}
	catch (std::exception &e)
	{
		// clean everything before closing server
		cleanUp();
		throw ;
	}
}


/* ==================================================================================
    * Functions called when server is closing
================================================================================== */

Server::~Server(void)
{
	// clean everything before closing server
	cleanUp();
}

// Function to clean everything before closing server
void	Server::cleanUp(void)
{
	// free parser
	delete _parser;

	// close server fd
	close(_fd);

	// close cliens fd
	for (unsigned int i = 1; i < _pollfds.size(); i++)
		close(_pollfds[i].fd);
}

/* ==================================================================================
    * Displaying errors
================================================================================== */

// A customized runtime_error
void	Server::throwRuntimeError(const std::string &prefixErrMsg) const
{
	std::string actualErrMsg(strerror(errno));
	throw std::runtime_error("ERROR: " + prefixErrMsg + " - " + actualErrMsg);
}

/* ==================================================================================
    * Functions to handle signals
================================================================================== */

// Set all signal handlers
void	Server::setUpSignalHandlers(void)
{
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
}

// Set _shutDown to true if SIGINT or SIGQUIT is detected
void	Server::signalHandler(int sigNum)
{
	std::cout << "INFO: Server is closing" << std::endl;
	if (sigNum == SIGINT || sigNum == SIGQUIT)
		_shutDown = true;
}

// To know if the server has shut down 
bool	Server::hasShutDown(void)
{
	return (_shutDown);
}

/* ==================================================================================
    * Initialize and run the server
================================================================================== */

// Initialize the server
void	Server::init(void)
{
	Logger::logInfo(WHITE, "Server initialization started");

	// Creation of the socket for the server
	if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throwRuntimeError("Creation of socket failed");


	// To make I/O operations non-blocking
	timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
		throwRuntimeError("Set socket option SO_RCVTIMEO failed");
	if (setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
		throwRuntimeError("Set socket option SO_SNDTIMEO failed");
	
	// Binding the socket to the network address
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_config.getPort());
	if (bind(_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
		throwRuntimeError("Binding failed");
	
	// Listening for incoming connections
	if (listen(_fd, SOMAXCONN) < 0)
		throwRuntimeError("Listening failed");

	// Setting up the server fd to be monitored
	_pollfds.resize(1);
	_pollfds[0].fd = _fd;
	_pollfds[0].events = POLLIN;
	_pollfds[0].revents = 0;
	
	Logger::logInfo(WHITE, "Server initialization done");
}

// Run the server
void	Server::run(void)
{
	Logger::logInfo(WHITE, "Server is running...");
	while (_shutDown == false)
	{		
		// Poll events on file descriptors
		if (poll(&_pollfds[0], _pollfds.size(), -1) < 0)
		{
			// If SIGINT or SIGQUIT while polling events
			if (errno == EINTR)
				return ;
				
			// if other types of error, throw an error
			else
				throwRuntimeError("Polling events failed");
		}
		
		// Check for existing incoming data on server fd
		if (_pollfds[0].revents & POLLIN)
			handleNewClient();
		
		// Handling all new clients requests
		for (unsigned int i = 1; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents & (POLLIN | POLLERR | POLLOUT))
				handleEvents(_pollfds[i]);
			if (_shutDown == true)
				return ;
		}	
	}	
}

/* ==================================================================================
    * Functions needed when server is running
================================================================================== */

// Handle new client connection to the server
void Server::handleNewClient(void)
{
	int		clientFd;
	int 	addrSize = sizeof(_address);
	pollfd 	clientPollfd;
	
	// Check if server can handle more clients
	if (static_cast<unsigned int>(_pollfds.size()) - 1 >= SOMAXCONN)
	{
		Logger::logInfo(WHITE, "Client not accepted. Maximum number of clients has been reached");
		return ;
	}
	
	// Accept new client connection
	if ((clientFd = accept(_fd, (struct sockaddr *)&_address, (socklen_t*)&addrSize)) < 0)
		throwRuntimeError("Accepting new client failed");

	Logger::logInfo(WHITE, "New client connection accepted");
	
	// Add new client fd to the poll set
	clientPollfd.fd = clientFd;
	clientPollfd.events = POLLIN | POLLOUT | POLLERR;
	clientPollfd.revents = 0;
	_pollfds.push_back(clientPollfd);

	// Add this new client to the map of clients
	_clients.insert(std::make_pair(clientFd, Client(clientPollfd)));

	Logger::logPollSet(_pollfds);
}

// Get the client associated with the fd
Client *Server::getClient(int fd)
{
	std::map<int, Client>::iterator it;

	it = _clients.find(fd);
	if (it != _clients.end())
		return (&it->second);
	return (NULL);
}

// Handling all events that happens on the client fd
void Server::handleEvents(pollfd clientPollfd)
{
	Client *client;

	// Retrieve the information about the client
	client = getClient(clientPollfd.fd);
	if (client == NULL)
		throw std::runtime_error("ERROR: handleEvents() - Client not found");

	// if an error occurred with client fd, remove it from server
	if (clientPollfd.revents & POLLERR)
		return (Logger::logInfo(WHITE, "Error detected on a client fd"), removeClient(client));

	// If client fd is ready for writing, and server pending msgs exist
	// try to send them all
	if ((clientPollfd.revents & POLLOUT) && !(client->getPendingServerMsgs().empty()))
		sendAllPendingMsgsToClient(client);
	
	// If client request has been detected, handle it
	if (clientPollfd.revents & POLLIN)
		handleClientRequest(client);
}

// Handle client requests
void	Server::handleClientRequest(Client *client)
{
	int			bytesRead;
	char		buffer[BUFFER_SIZE] = {0};
	int			clientFd;
	
	Logger::logInfo(WHITE, "Handling client request");
	if (client->getNickname() != "")
		Logger::logValue(BRIGHT_RED, "[SERVER] client", client->getNickname());
	
	clientFd = client->getFd();
	if (_shutDown == false)
	{
		// Get the client request
		bytesRead = recv(clientFd, buffer, BUFFER_SIZE, 0);
		buffer[bytesRead] = 0;

		// If client is momentarily unavailable, try later to read incoming data
		if ((bytesRead == -1)
			&& (errno == EAGAIN || errno == EWOULDBLOCK))
			return ;
		
		// If other errors than EAGAIN or EWOULDBLOCK, remove client from server
		else if (bytesRead <= 0)
		{
			Logger::logInfo(WHITE, "Client is disconnected. Server will remove it");
			removeClient(client);
		}
		// Handle the request received
		else
		{
			client->updateRequest(std::string(buffer));

			Logger::logBeforeParsing(BRIGHT_RED, std::string(buffer), client->getRequest());
			
			// Parse and execute the client request
			try
			{
				_parser->parseRequest(client);
			}
			catch (const std::exception &e)
			{
				std::cerr << e.what() << std::endl;
			}
			
			// If client has sent a QUIT request, remove it from the server
			if (client->getHasQuit() == true)
			{
				Logger::logInfo(WHITE, "Client is disconnected. Server will remove it");
				removeClient(client);
			}
			// If client has not quit, log his buffer
			else
				Logger::logAfterParsing(BRIGHT_RED, buffer, client->getRequest());
		}
	}
}

// Remove a client from the server
void Server::removeClient(Client *client)
{
	int fd = client->getFd();
	
	// Close client fd
	close(fd);

	// Remove client from the poll set
	std::vector<pollfd>::iterator itVec;
	for (itVec = _pollfds.begin(); itVec != _pollfds.end(); itVec++)
	{
		if ((*itVec).fd == fd)
		{
			_pollfds.erase(itVec);
			break ;
		}
	}

	// Remove client from all channels
	std::map<std::string, Channel>::iterator itChannelMap;
	for (itChannelMap = _channels.begin(); itChannelMap != _channels.end(); itChannelMap++)
	{
		itChannelMap->second.removeClient(client->getNickname());
		itChannelMap->second.removeOperator(client->getNickname());
		itChannelMap->second.removeClientInvited(client->getNickname());
	}

	// Remove client from server map
	std::map<int, Client>::iterator itClientMap;
	itClientMap = _clients.find(fd);
	if (itClientMap != _clients.end())
		_clients.erase(itClientMap);
	
	Logger::logPollSet(_pollfds);
}

// Function that will be used for the server to send a message to a client
void	Server::handleNewMsgToClient(const std::string &msg, Client *client)
{
	// enqueue the new message
	client->addNewServerMsg(msg);

	// now try to send all messages (pending ones + this new one)
	if (client->getPollfd().revents & POLLOUT)	
		sendAllPendingMsgsToClient(client);
}

// Function to send all pending messags to a client
void	Server::sendAllPendingMsgsToClient(Client *client)
{
	ssize_t 				bytesSent;
	std::queue<std::string> &pendingMsgs = client->getPendingServerMsgs();

	// Logger::logQueue(LIGHT_RED,"[SERVER] BEFORE LOOP - pending server messages",pendingMsgs);

	// As long as there are pending msgs, try to send them
	while ((!(pendingMsgs.empty())) && _shutDown == false)
	{
		// Get each msg and try to send it
		std::string &currentMsg = pendingMsgs.front();

		// Logger::logValue(LIGHT_RED, "[SERVER] message to send", currentMsg);

		bytesSent = send(client->getFd(), currentMsg.c_str(), currentMsg.size(), 0);
		
		// ---- If sending one msg failed, properly handle the error ---
		
		// If client is momentarily unavailable, try later to send it
		if ((bytesSent == -1)
			&& (errno == EAGAIN || errno == EWOULDBLOCK))
			break ;
		// if other errors than EAGAIN or EWOULDBLOCK, remove client from server
		else if (bytesSent == -1)
		{
			removeClient(client);
			break ;
		}
		// if the entirety of the msg was not sent, try with the remainder later
		else if (bytesSent < static_cast<ssize_t>(currentMsg.size()))
		{
			currentMsg = currentMsg.substr(bytesSent);
			break;
		}

		// remove this current message from pending msgs
		pendingMsgs.pop();
	}

	// Logger::logQueue(LIGHT_RED,"[SERVER] AFTER LOOP - pending server messages",pendingMsgs);

}

/* ==================================================================================
    * Functions needed for other classes
================================================================================== */

// Get the client associated with the nickname
Client *Server::getClient(const std::string &nickname)
{
	std::map<int, Client>::iterator it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second.getNickname() == nickname)
			return (&(it->second));
	}
	return (NULL);
}

// Get all clients
std::map<int, Client>	Server::getClients(void) const
{
	return _clients;
}

// Get configuration data
Configuration Server::getConfig(void) const
{
	return _config;
}

// Get a specific channel
Channel	*Server::getChannel(std::string channelName) const
{
	// Logger::logMapKey(BRIGHT_RED, "[SERVER] Display channels in getChannel()", _channels);
	channelName = Utils::toLowerChannelName(channelName);
	std::map<std::string, Channel>::const_iterator it = _channels.find(channelName);
	if (it != _channels.end())
		return const_cast<Channel*>(&it->second);
	// Logger::log(BRIGHT_RED, "[SERVER] no channel with that name found");
	return NULL;
}

// Add a new channel to the server
void	Server::addNewChannel(std::string channelName, const std::string &creatorName)
{
	channelName = Utils::toLowerChannelName(channelName);
	Logger::logValue(LIGHT_GREEN, "[SERVER] try to add channel", channelName);

	_channels.insert(std::make_pair(channelName, Channel(channelName, creatorName)));
	Logger::logMapKey(LIGHT_GREEN, "[SERVER] _channels in addNewChannel()", _channels);
}

// Broadcast a message to members of a specific channel
void	Server::broadcastMsgToAllChannelMembers(Channel *channel, const std::string &msg)
{
	std::vector<std::string>			clientsName = channel->getClients();
	std::vector<std::string>::iterator	it;
	Client								*client;

	for (it = clientsName.begin(); it != clientsName.end(); it++)
	{
		client = getClient(*it);
		handleNewMsgToClient(msg, client);
	}
}

void	Server::broadcastMsgToAllMembers(const std::string &msg)
{
	std::map<int, Client>::iterator	it;

	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		handleNewMsgToClient(msg, &(it->second));
	}
}

std::vector<std::string> Server::getNicknameFromUsername(const std::string &username) const
{
	std::vector<std::string> nicknames;
	std::map<int, Client>::const_iterator it = _clients.begin();
	for ( ; it != _clients.end(); ++it)
	{
		if (it->second.getUsername() == username)
		{
			nicknames.push_back(it->second.getNickname());
		}
	}
	return nicknames;
}
