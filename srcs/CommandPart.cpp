/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandPart.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 23:06:53 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/11 21:35:21 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	RFC 2812
	
	Command: PART
   	Parameters: <channel> *( "," <channel> ) [ <Part Message> ]

	The PART command causes the user sending the message to be removed
	from the list of active members for all given channels listed in the
	parameter string.

	Numeric Replies:

			ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
			ERR_NOTONCHANNEL
	*/

/* ==================================================================================
    * Constructor
================================================================================== */

#include "CommandPart.hpp"

CommandPart::CommandPart(std::vector<std::string> params, Client *client, Server *server)
	: ACommand(params, client, server)
{
	Logger::logVector(LIGHT_RED, "[PART] _params", _params);

	// Check if client who issued the command is registered
	checkIsRegistered();

	// Check the number of parameters provided (ERR_NEEDMOREPARAMS)
	checkParamsNb("PART", 1, 2);

	// Parse the parameters
	_channelNames = splitParametersList(_params[0]);

	// RFC 2812: "If a "Part Message" is given, this will be sent
  	// instead of the default message, the nickname."
	if (_params.size() == 2)
		_comment = _params[1];
	else
		_comment = _client->getNickname();

}

/* ==================================================================================
    * Execution
================================================================================== */

void	CommandPart::execute(void)
{
	std::vector<std::string>::iterator	it;

	for (it = _channelNames.begin(); it != _channelNames.end(); it++)
	{
		try
		{
			Logger::logValue(LIGHT_RED, "[PART] current channel name BEFORE executing PART", *it);
			
			// Try to get the channel (ERR_NOSUCHCHANNEL)
			_channel = getChannel(*it);

			Logger::logValue(LIGHT_RED, "[PART] channel info", _channel->getListOfAllUsersNamesInChannelAsStr());

			
			// Check if the client who issued the command is inside the channel provided (ERR_NOTONCHANNEL)
			isNicknameInChannel(_client->getNickname(), *it, true);

			// Remove the client from the channel
			_channel->removeClient(_client->getNickname());
			_channel->removeOperator(_client->getNickname());

			// Notify the client who issued the command that PART was executed for the channel
			_serverReply = ":" + _client->getHostmask() + " PART " + *it + " :" + _comment + "\r\n";
			_server->handleNewMsgToClient(_serverReply, _client);
			
			// Notify all channel members
			_server->broadcastMsgToAllChannelMembers(_channel, _serverReply);
		}
		catch (const std::exception &e)
		{
			std::cerr << "ERROR: PART command - " << e.what() << std::endl;
			_channel = NULL;
		}
		if (_channel != NULL)
		{
			Logger::logValue(LIGHT_RED, "[PART] current channel name", *it);
			Logger::logValue(LIGHT_RED, "[PART] channel info AFTER execution part", _channel->getListOfAllUsersNamesInChannelAsStr());
		}
	}	
}
