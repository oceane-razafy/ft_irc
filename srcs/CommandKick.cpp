/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandKick.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/01 18:38:40 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/19 14:18:44 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandKick.hpp"

/*
	RFC 2812

    Command: KICK
   	Parameters: <channel> *( "," <channel> ) <user> *( "," <user> )
               [<comment>]

	The KICK command can be used to request the forced removal of a user
   	from a channel.

	Numeric Replies:

			ERR_NEEDMOREPARAMS              		ERR_NOSUCHCHANNEL
			ERR_BADCHANMASK (not in the subject) 	ERR_CHANOPRIVSNEEDED
			ERR_USERNOTINCHANNEL            		ERR_NOTONCHANNEL
*/

/* ==================================================================================
    * Constructor
================================================================================== */

CommandKick::CommandKick(std::vector<std::string> params, Client *client, Server *server)
	: ACommand(params, client, server)
{
	Logger::logVector(LIGHT_BLUE, "[KICK] _params in CommandKick()", _params);
	
	// Check if client who issued the command is registered
	checkIsRegistered();

	// Check the number of parameters provided (ERR_NEEDMOREPARAMS)
	checkParamsNb("KICK", 2, 3);

	// Parse the parameters
	_channelNames = splitParametersList(_params[0]);
	_clientsToKick = splitParametersList(_params[1]);
	
	// RFC 2812: "If a "comment" is given, this will be sent instead of the default message,
	// the nickname of the user issuing the KICK"
	if (_params.size() == 3)
		_comment = _params[2];
	else
		_comment = _client->getNickname();

	// RFC 2812: "For the message to be syntactically correct, there MUST be
	// either one channel parameter and multiple user parameter, or as many
	// channel parameters as there are user parameters." (ERR_NEEDMOREPARAMS)
	if (_channelNames.size() != 1)
	{
		if (_channelNames.size() != _clientsToKick.size())
		{
			_serverReply = ErrorReplies::errNeedMoreParams(_client->getNickname(), "KICK");
			_server->handleNewMsgToClient(_serverReply, _client);
			throw NeedMoreParamsException();
		}	
	}
}

/* ==================================================================================
    * Execution
================================================================================== */

void	CommandKick::execute(void)
{
	std::vector<std::string>::iterator itChan;
	std::vector<std::string>::iterator itEjectedClient;

	for (size_t i = 0; i < _channelNames.size(); i++)
	{
		try
		{
			Logger::logValue(LIGHT_BLUE, "[KICK] current channel name BEFORE executing KICK", _channelNames[i]);
			
			// Try to get the channel (ERR_NOSUCHCHANNEL)
			_channel = getChannel(_channelNames[i]);

			Logger::logValue(LIGHT_BLUE, "[KICK] channel info", _channel->getListOfAllUsersNamesInChannelAsStr());

			// Check if the client who issued the command is inside the current channel (ERR_NOTONCHANNEL)
			isNicknameInChannel(_client->getNickname(), _channelNames[i], true);
			
			// If one channel name parameter with multiple clients parameters
			if (_channelNames.size() == 1)
			{
				for (size_t j = 0; j < _clientsToKick.size(); j++)
				{
					try
					{
						// Try to kick the client
						tryToKickClient(_clientsToKick[j], _channelNames[i]);

						// Notify the client kicked
						_serverReply = ":" + _client->getHostmask() + " KICK " + _channelNames[i] + " " + _clientsToKick[j] + " :" + _comment + "\r\n";
						_server->handleNewMsgToClient(_serverReply, _server->getClient(_clientsToKick[j]));
						
						// Notify all channel members
						_server->broadcastMsgToAllChannelMembers(_channel, _serverReply);
					}
					catch(const Channel::ClientNotAnOperatorOfThisChannelException &e)
					{
						std::cerr << "ERROR: KICK command - " << e.what() << std::endl;
						break ;
					}
					catch(const ACommand::TargetNotInChannelException &e)
					{
						std::cerr << "ERROR: KICK command - " << e.what() << std::endl;
					}	
				}
			}
			// If same number of channel name parameters as number of clients to kick parameters
			else
			{
				// Try to kick the client
				tryToKickClient(_clientsToKick[i], _channelNames[i]);

				// Notify the client kicked
				_serverReply = ":" + _client->getHostmask() + " KICK " + _channelNames[i] + " " + _clientsToKick[i] + " :" + _comment + "\r\n";
				_server->handleNewMsgToClient(_serverReply, _server->getClient(_clientsToKick[i]));
				
				// Notify all channel members
				_server->broadcastMsgToAllChannelMembers(_channel, _serverReply);
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << "ERROR: KICK command - " << e.what() << std::endl;
			_channel = NULL;
		}
		if (_channel != NULL)
		{
			Logger::logValue(LIGHT_BLUE, "[KICK] current channel name", _channelNames[i]);
			Logger::logValue(LIGHT_BLUE, "[KICK] channel info AFTER kicking", _channel->getListOfAllUsersNamesInChannelAsStr());
		}
	}
}

// Kick a specific client from a specific channel
void	CommandKick::tryToKickClient(const std::string &clientToKick, std::string &channelName)
{
	// Check if the target client provided is inside the current channel (ERR_USERNOTINCHANNEL )
	isNicknameInChannel(clientToKick, channelName, false);

	// Try to eject the target client
	try
	{
		_channel->ejectClient(_client->getNickname(), clientToKick);
		_clientToKick = _server->getClient(clientToKick);
		_clientToKick->removeChannel(channelName);
	}
	// if the client who issued the command does not have the operator privilege, send an error reply (ERR_CHANOPRIVSNEEDED)
	catch (const Channel::ClientNotAnOperatorOfThisChannelException &e)
	{
		_serverReply = ErrorReplies::errChanoPrivsNeeded(_client->getNickname(), channelName);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NotOperatorException();
	}
}
