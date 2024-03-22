/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandQuit.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 18:42:42 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/19 13:27:41 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandQuit.hpp"

/*
	RFC 2812

	Command: QUIT
   	Parameters: [ <Quit Message> ]

   	A client session is terminated with a quit message.
	
  	Numeric Replies:

           None.
*/

/* ==================================================================================
    * Constructor
================================================================================== */

CommandQuit::CommandQuit(std::vector<std::string> params, Client *client, Server *server)
	: ACommand(params, client, server)
{
	Logger::logVector(LIGHT_ORANGE, "[QUIT] _params", _params);
	
	// Check if client who issued the command is registered
	checkIsRegistered();

	// Check the number of parameters provided (ERR_NEEDMOREPARAMS)
	checkParamsNb("QUIT", 0, 1);

	// Take the quit message if it does exist
	if (_params.size() == 1)
		_comment = _params[0];

	// Build the message the server will broadcast to all clients that share same channels
	_serverReply = ":" + _client->getHostmask() + " QUIT :" + _comment + "\r\n";
}

/* ==================================================================================
    * Execution
================================================================================== */

void	CommandQuit::execute(void)
{
	std::vector<std::string>			channelNames = _client->getChannels();
	std::vector<std::string>::iterator	itChan;
	std::vector<std::string>			clientsName;
	std::vector<std::string>::iterator	itClient;
	Client								*client;
	std::vector<std::string>			clientsDone;

	for (itChan = channelNames.begin(); itChan != channelNames.end(); itChan++)
	{
		Logger::logValue(LIGHT_ORANGE, "[QUIT] current channel name BEFORE executing QUIT", *itChan);

		// Get each channel
		_channel = _server->getChannel(*itChan);

		Logger::logValue(LIGHT_ORANGE, "[QUIT] channel info", _channel->getListOfAllUsersNamesInChannelAsStr());

		// Remove the client from the channel
		_channel->removeClient(_client->getNickname());
		_channel->removeClientInvited(_client->getNickname());
		_channel->removeOperator(_client->getNickname());
		
		// Notify all channel members
		clientsName = _channel->getClients();
		for (itClient = clientsName.begin(); itClient != clientsName.end(); itClient++)
		{
			// Get each client of a channel
			client = _server->getClient(*itClient);
			
			// To avoid duplicate to a client that is a member of multiple common channels
			if (std::find(clientsDone.begin(), clientsDone.end(), client->getNickname()) == clientsDone.end())
			{
				_server->handleNewMsgToClient(_serverReply, client);
				clientsDone.push_back(client->getNickname());
			}
		}
		Logger::logValue(LIGHT_ORANGE, "[QUIT] current channel name", *itChan);
		Logger::logValue(LIGHT_ORANGE, "[QUIT] channel info AFTER quitting", _channel->getListOfAllUsersNamesInChannelAsStr());
	}
	
	// RFC 2812: "The server acknowledges this by sending an ERROR message to the client."
	_serverReply = ":" + serverName + " ERROR: You have quit. Your session is going to be terminated" + "\r\n";
	_server->handleNewMsgToClient(_serverReply, _client);

	// Flag that the client has quit to remove it from the server
	_client->setHasQuit(true);
}
