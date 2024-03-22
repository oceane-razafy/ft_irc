/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandInvite.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 17:00:35 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/08 21:01:40 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandInvite.hpp"

/*
	RFC 2812
	
    Command: INVITE
   	Parameters: <nickname> <channel>

	The INVITE command is used to invite a user to a channel.

	Numeric Replies:
			ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
			ERR_NOTONCHANNEL                ERR_USERONCHANNEL
			ERR_CHANOPRIVSNEEDED
			RPL_INVITING                    RPL_AWAY (not in the subject)
*/

/* ==================================================================================
    * Constructor
================================================================================== */

CommandInvite::CommandInvite(std::vector<std::string> params, Client *client, Server *server)
	: ACommand(params, client, server)
{
	Logger::logVector(GREEN, "[INVITE] _params", _params);

	// Check if client who issued the command is registered
	checkIsRegistered();

	// Check the number of parameters provided (ERR_NEEDMOREPARAMS)
	checkParamsNb("INVITE", 2, 2);

	// Check if the nickname provided exists in the server (ERR_NOSUCHNICK)
	isNicknameInServer(_params[0]);

	/*
		NOTE: we don't use the getChannel() from ACommand
		because in RFC 2812 it's written: "There is no requirement that the
   		channel the target user is being invited to must exist or be a valid
   		channel." -> so no exception or error reply NoSuchChannel needs to happen
	*/
	_channel = _server->getChannel(_params[1]);

	if (_channel != NULL)
	{
		Logger::log(GREEN, "[INVITE] The channel does exist");
		Logger::logValue(GREEN, "[INVITE] current channel name", _channel->getName());
		Logger::logValue(GREEN, "[INVITE] channel info", _channel->getListOfAllUsersNamesInChannelAsStr());
		
		// Check if the client who issued the command is inside the channel provided (ERR_NOTONCHANNEL)
		isNicknameInChannel(_client->getNickname(), _params[1], true);

		// Check if the target of the command is already inside the channel provided (ERR_USERONCHANNEL)
		if (_channel->isMember(_params[0]))
		{
			_serverReply = ErrorReplies::errUserOnChannel(_client->getNickname(), _params[0], _params[1]);
			_server->handleNewMsgToClient(_serverReply, _client);
			throw UserOnChannelException();
		}
	}
	else
		Logger::log(GREEN, "[INVITE] The channel does NOT exist");

	// Check if the client who issued the command has the operator privilege to do it:
	// If the channele does exits
	// and the channel is invite-only
	// and the client who issued the command is not an operator 
	// => server sends an error reply to the client (ERR_CHANOPRIVSNEEDED)
	// => throw an exception
	if (_channel != NULL
		&& _channel->getIsInviteOnly() == true
		&& _channel->isOperator(client->getNickname()) == false)
	{
		_serverReply = ErrorReplies::errChanoPrivsNeeded(_client->getNickname(), _params[1]);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NotOperatorException();
	}
}

/* ==================================================================================
    * Execution
================================================================================== */

void	CommandInvite::execute(void)
{
	// Send the invitation to the target
	_serverReply = ":" + _client->getHostmask() + " INVITE " + _params[0] + " " + _params[1] + "\r\n";
	Client *target = _server->getClient(_params[0]);
	_server->handleNewMsgToClient(_serverReply, target);

	// Send RPL_INVITING to the client
	_serverReply = CommandResponses::rplInviting(_client->getNickname(), _params[1], _params[0]);
	_server->handleNewMsgToClient(_serverReply, _client);

	// Add the target name to the channel invited clients vector
	if (_channel != NULL)
		_channel->addInvitedClients(target->getNickname());
}

const char* CommandInvite::UserOnChannelException::what() const throw()
{
	return ("User already on the channel");
}
