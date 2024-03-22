/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandTopic.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 03:42:32 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/20 14:37:55 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	RFC 2812

	Command: TOPIC
   	Parameters: <channel> [ <topic> ]

   	The TOPIC command is used to change or view the topic of a channel.

	Numeric Replies:

			ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
			RPL_NOTOPIC                     RPL_TOPIC
			ERR_CHANOPRIVSNEEDED            ERR_NOCHANMODES (not in the subject)
*/

#include "CommandTopic.hpp"

CommandTopic::CommandTopic(std::vector<std::string> params, Client *client, Server *server)
	: ACommand(params, client, server)
{
	Logger::logVector(LIGHT_PURPLE, "[TOPIC] _params", _params);

	// Check if client who issued the command is registered
	checkIsRegistered();

	// Check the number of parameters provided (ERR_NEEDMOREPARAMS)
	checkParamsNb("TOPIC", 1, 2);

	// Check if the client who issued the command is inside the channel provided (ERR_NOTONCHANNEL)
	isNicknameInChannel(client->getNickname(), _params[0], true);
	
	// We try to get the topic of the channel
	_channel = _server->getChannel(_params[0]);
	try
	{
		_topic = _channel->viewTopic(_client->getNickname());
	}
	// if the client who issued the command does not have the operator privilege, send an error reply (ERR_CHANOPRIVSNEEDED)
	catch (const Channel::ClientNotAnOperatorOfThisChannelException &e)
	{
		_serverReply = ErrorReplies::errChanoPrivsNeeded(_client->getNickname(), _params[0]);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NotOperatorException();
	}
}

void	CommandTopic::execute(void)
{
	// RFC 2812: "The topic for channel <channel> is returned if there is no <topic> given"
	// No topic parameter was given
	if (_params.size() == 1)
	{
		if (!(_topic.empty()))
		{
			// If topic of the channel is not empty, RPL_TOPIC will be sent
			_serverReply = CommandResponses::rplTopic(_client->getNickname(), _params[0], _topic);
			_server->handleNewMsgToClient(_serverReply, _client);
		}
		else
		{
			// If topic of the channel is empty, RPL_NOTOPIC will be sent
			_serverReply = CommandResponses::rplNoTopic(_client->getNickname(), _params[0]);
			_server->handleNewMsgToClient(_serverReply, _client);
		}
	}

	// A topic parameter was given
	else
	{
		// Set the new topic
		_channel->setTopic(_client->getNickname(), _params[1]);
		
		// Get the new topic set
		_topic = _channel->viewTopic(_client->getNickname());

		if (_topic.empty())
		{
			// If topic that has been set is empty, RPL_NOTOPIC will be sent
			_serverReply = CommandResponses::rplNoTopic(_client->getNickname(), _params[0]);
			_server->handleNewMsgToClient(_serverReply, _client);

			// Notify all channel members
			_serverReply = ":" + _client->getHostmask() + " TOPIC " + _params[0] + " :\r\n";
			_server->broadcastMsgToAllChannelMembers(_channel, _serverReply);
		}
		else
		{
			// If topic that has been set is not empty, RPL_TOPIC will be sent
			_serverReply = CommandResponses::rplTopic(_client->getNickname(), _params[0], _topic);
			_server->handleNewMsgToClient(_serverReply, _client);

			// Notify all channel members
			_serverReply = ":" + _client->getHostmask() + " TOPIC " + _params[0] + " " + _params[1] + "\r\n";
			_server->broadcastMsgToAllChannelMembers(_channel, _serverReply);
		}
	}
}
