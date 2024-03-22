/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandJoin.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: valentin <valentin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 22:36:23 by valentin          #+#    #+#             */
/*   Updated: 2024/03/14 13:39:30 by valentin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandJoin.hpp"

/*
	RFC 2812

	Command: JOIN
   	Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] )
               / "0"

	The JOIN command is used by a user to request to start listening to
   	the specific channel. 

	Numeric Replies:

		ERR_NEEDMOREPARAMS             				ERR_BANNEDFROMCHAN (not in the subject)
		ERR_INVITEONLYCHAN             				ERR_BADCHANNELKEY
		ERR_CHANNELISFULL              				ERR_BADCHANMASK (not in the subject)
		ERR_NOSUCHCHANNEL (not used, when channel 	ERR_TOOMANYCHANNELS
		does not exist, we create new one)     		ERR_UNAVAILRESOURCE (not in the subject)
		ERR_TOOMANYTARGETS (not in the subject)		
		RPL_TOPIC
*/

/* ==================================================================================
    * Constructor
================================================================================== */

CommandJoin::CommandJoin(std::vector<std::string> params, Client *client, Server *server)
	: ACommand(params, client, server)
{
	Logger::logVector(LIGHT_PURPLE, "[JOIN] _params", _params);
	
	checkIsRegistered();

	checkParamsNb("JOIN", 1, 2);

	// Logger::logVector(BRIGHT_PURPLE, "[JOIN] Display params in CommandJoin", _params); 

	if (_params[0] == "0") // Leave all currently joined channels
	{
		// Logger::logDebug(BRIGHT_PURPLE, "Should leave all channels !");
		_shouldLeaveAllChannels = true; // Used in execute function
	}
	else
	{
		_shouldLeaveAllChannels = false;

		std::vector<std::string> channelsNames = splitParametersList(_params[0]);

		std::vector<std::string> triedkeys;
		if (_params.size() == 2)
			triedkeys = splitParametersList(_params[1]);

		if (triedkeys.size() > channelsNames.size())
		{
			_serverReply = ErrorReplies::errTooManyParams(_client->getNickname(), "JOIN");
			_server->handleNewMsgToClient(_serverReply, _client);
			throw TooManyParamsException();
		}

		_joinInfos = _createJoinInfo(channelsNames, triedkeys);
	}
}

/* ==================================================================================
    * Parsing the parameters
================================================================================== */

/*
	IMPORTANT EXAMPLES:

	JOIN 0

		; Leave all currently joined channels.

	JOIN #foo,#bar fubar,foobar

		; Command to join channel #foo using
		key "fubar", and channel #bar using
		key "foobar".
*/

std::vector<JoinInfo> CommandJoin::_createJoinInfo(const std::vector<std::string>& channelsNames, const std::vector<std::string>& triedKeys)
{
	std::vector<JoinInfo> joinInfos;

	for (size_t i = 0; i < channelsNames.size(); ++i)
	{
		JoinInfo info;
		info.channelName = channelsNames[i];

		if (i < triedKeys.size())
			info.triedKey = triedKeys[i];
		else
			info.triedKey = "";

		joinInfos.push_back(info);
	}

	return joinInfos;
}


/* ==================================================================================
    * Execution
================================================================================== */

void CommandJoin::execute(void)
{
	if (_shouldLeaveAllChannels)
		return _leaveAllChannels();

	std::vector<JoinInfo>::iterator it = _joinInfos.begin();
	for ( ; it != _joinInfos.end(); ++it)
	{
		Logger::logValue(LIGHT_PURPLE, "[JOIN] current channel name BEFORE executing JOIN", it->channelName);
		Channel *currentChannel = _server->getChannel(it->channelName);
		
		if (currentChannel == NULL)
		{
			Logger::log(LIGHT_PURPLE, "[JOIN] Channel does not exist");
			try
			{
				Logger::logValue(LIGHT_PURPLE, "[JOIN] try to create channel", it->channelName);
				
				_createChannelProcess(it->channelName);
			}
			catch (const Channel::InvalidChannelNameException &e)
			{
				_serverReply = ":" + serverName + " ERROR: Invalid channel name format" + "\r\n";
				_server->handleNewMsgToClient(_serverReply, _client);
				std::cerr << e.what() << std::endl;
			}
			continue ;
		}
		else
			Logger::logValue(LIGHT_PURPLE, "[JOIN] channel info", currentChannel->getListOfAllUsersNamesInChannelAsStr());

		if (currentChannel->getIsInviteOnly())
		{
			Logger::log(LIGHT_PURPLE, "[JOIN] Channel is invite-only");
			if (!currentChannel->isClientInvited(_client->getNickname())) {
				_serverReply = ErrorReplies::errInviteOnlyChan(_client->getNickname(), it->channelName);
				_server->handleNewMsgToClient(_serverReply, _client);
				continue ;
			}
		}
		else
			Logger::log(LIGHT_PURPLE, "[JOIN] Channel is NOT invite-only");

		// If there's a channel key set
		if (currentChannel->isKeySet())
		{
			Logger::log(LIGHT_PURPLE, "[JOIN] Channel has a key set");
			if (!currentChannel->isChannelKey(it->triedKey))
			{
				_serverReply = ErrorReplies::errBadChannelKey(_client->getNickname(), it->channelName);
				_server->handleNewMsgToClient(_serverReply, _client);
				std::cerr << "ERROR: JOIN command - Bad channel key provided" << std::endl;
				continue ;
			}
		}
		else
			Logger::log(LIGHT_PURPLE, "[JOIN] Channel does NOT have a key set");

		try
		{
			Logger::log(LIGHT_PURPLE, "[JOIN] Executing the join process");
			_joinChannelProcess(currentChannel, it->channelName);
			currentChannel->removeClientInvited(_client->getNickname());
		}
		catch (Channel::MaxNbClientsReachedException & e)
		{
			_serverReply = ErrorReplies::errChannelIsFull(_client->getNickname(), it->channelName);
			_server->handleNewMsgToClient(_serverReply, _client);
			std::cout << "ERROR - JOIN command - " << e.what() << std::endl; 
		}
	}
}

// Client leaves all channels ("JOIN 0")
void CommandJoin::_leaveAllChannels(void)
{
	Logger::log(LIGHT_PURPLE, "[JOIN] Processing _leaveAllChannels");
	
	std::vector<std::string> currentChannels = _client->getChannels();
	std::vector<std::string>::iterator it = currentChannels.begin();
	for ( ; it != currentChannels.end(); ++it)
	{
		Channel *channel = _server->getChannel(*it);
		
		Logger::logValue(LIGHT_PURPLE, "[JOIN] current channel name in _leaveAllChannels()", *it);
		Logger::logValue(LIGHT_PURPLE, "[JOIN] channel info", channel->getListOfAllUsersNamesInChannelAsStr());
		
		channel->removeClient(_client->getNickname());
		channel->removeOperator(_client->getNickname());
		_client->removeChannel(*it);
		_serverReply = ":" + _client->getHostmask() + " PART " + *it + "\r\n";
		_server->broadcastMsgToAllChannelMembers(channel, _serverReply);

		Logger::logValue(LIGHT_PURPLE, "[JOIN] current channel name", *it);
		Logger::logValue(LIGHT_PURPLE, "[JOIN] channel info AFTER leaving all channels", channel->getListOfAllUsersNamesInChannelAsStr());
	}
}

// Client joins an existing channel
void CommandJoin::_joinChannelProcess(Channel *currentChannel, std::string channelName)
{
	Logger::logValue(LIGHT_PURPLE, "[JOIN] current channel name in _joinChannelProcess()", channelName);
	Logger::logValue(LIGHT_PURPLE, "[JOIN] channel info", currentChannel->getListOfAllUsersNamesInChannelAsStr());

	try
	{
		currentChannel->addClient(_client->getNickname());
		_client->addChannel(channelName);
	}
	catch (const Channel::AlreadyInChannelException &e)
	{
		std::cerr << e.what() << std::endl;
		std::string rplAlreadyInChannel = ":" + serverName + " ERROR: Already in channel " + channelName + ", cannot join twice.\r\n";
		_server->handleNewMsgToClient(rplAlreadyInChannel, _client);
		return ;
	}

	_serverReply = ":" + _client->getHostmask() + " JOIN " + channelName + "\r\n";
	_server->broadcastMsgToAllChannelMembers(currentChannel, _serverReply);

	_serverReply = CommandResponses::rplTopic(_client->getNickname(), channelName, currentChannel->getTopic());
	_server->handleNewMsgToClient(_serverReply, _client);

	std::string rplNamreply = ":" + serverName + " 353 " + _client->getNickname() + " = " + channelName + " :" + currentChannel->getListOfAllUsersNamesInChannelAsStr() + "\r\n";
	_server->handleNewMsgToClient(rplNamreply, _client);

	std::string rplEndOfNames = ":" + serverName + " 366 " + _client->getNickname() + " " + channelName + " :End of /NAMES list\r\n";
	_server->handleNewMsgToClient(rplEndOfNames, _client);

	Logger::logValue(LIGHT_PURPLE, "[JOIN] current channel name in _joinChannelProcess()", channelName);
	Logger::logValue(LIGHT_PURPLE, "[JOIN] channel info AFTER joining", currentChannel->getListOfAllUsersNamesInChannelAsStr());
}

// Client joins a non-existing channel (server creates the channel)
void CommandJoin::_createChannelProcess(std::string channelName)
{
	Logger::logValue(LIGHT_PURPLE, "[JOIN] channel in _createChannelProcess", channelName);

	_server->addNewChannel(channelName, _client->getNickname());
	_client->addChannel(channelName);

	_serverReply = ":" + _client->getHostmask() + " JOIN " + channelName + "\r\n";
	_server->handleNewMsgToClient(_serverReply, _client);

	_serverReply = CommandResponses::rplTopic(_client->getNickname(), channelName, "");
	_server->handleNewMsgToClient(_serverReply, _client);

	std::string rplNamreply = ":" + serverName + " 353 " + _client->getNickname() + " = " + channelName + " :@" + _client->getNickname() + "\r\n";
	_server->handleNewMsgToClient(rplNamreply, _client);

	std::string rplEndOfNames = ":" + serverName + " 366 " + _client->getNickname() + " " + channelName + " :End of /NAMES list\r\n";
	_server->handleNewMsgToClient(rplEndOfNames, _client);

	Logger::logValue(LIGHT_PURPLE, "[JOIN] current channel name in _createChannelProcess()", channelName);
	Logger::logValue(LIGHT_PURPLE, "[JOIN] channel info AFTER creating new channel", _server->getChannel(channelName)->getListOfAllUsersNamesInChannelAsStr());
}
