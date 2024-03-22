/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandPrivmsg.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: valentin <valentin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 17:43:23 by valentin          #+#    #+#             */
/*   Updated: 2024/03/14 13:38:31 by valentin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandPrivmsg.hpp"
#include "Utils.hpp"

/*
	RFC 2812

	Command: PRIVMSG
   	Parameters: <msgtarget> <text to be sent>

   	PRIVMSG is used to send private messages between users, as well as to
  	send messages to channels.
	
 	Numeric Replies:

		ERR_NORECIPIENT                 		ERR_NOTEXTTOSEND
		ERR_CANNOTSENDTOCHAN           			ERR_NOTOPLEVEL (not in the subject)
		ERR_WILDTOPLEVEL (not in the subject)	ERR_TOOMANYTARGETS
		ERR_NOSUCHNICK
		RPL_AWAY (not in the subject)
*/

/* ==================================================================================
    * Constructor
================================================================================== */

CommandPrivmsg::CommandPrivmsg(std::vector<std::string> params, Client *client, Server *server)
	: ACommand(params, client, server)
{
	Logger::logVector(BRIGHT_PURPLE, "[PRIVMSG] _params", _params);

	checkIsRegistered();
	if (_params.size() == 0)
	{
		_serverReply = ErrorReplies::errNoRecipient(_client->getNickname(), "PRIVMSG");
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NoRecipientException();
	}
	if (_params.size() == 1)
	{
		_serverReply = ErrorReplies::errNoTextToSend(_client->getNickname());
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NoTextToSendException();
	}
	if (_params.size() > 2)
	{
		_serverReply = ErrorReplies::errTooManyParams(_client->getNickname(), "PRIVMSG");
		_server->handleNewMsgToClient(_serverReply, _client);
		throw TooManyParamsException();
	}
	_msgToInfo = _parseMsgTo(_params[0]);
	_msgToSend = _formatMsgToSend(_params[1]);
}

/* ==================================================================================
    * Parser
================================================================================== */

MsgToInfo CommandPrivmsg::_parseMsgTo(const std::string& msgto) {
	MsgToInfo info;
	size_t percentPos = msgto.find('%');
	size_t atPos = msgto.find('@');
	size_t exclamPos = msgto.find('!');

	if (msgto[0] == '#' || msgto[0] == '+' || msgto[0] == '!' || msgto[0] == '&')
	{ // <channelName>
		info.channelName = msgto;
		info.type = "channel";
	}
	else if (percentPos != std::string::npos && atPos != std::string::npos)
	{ // <username>%<hostname>@<servername>
		info = _checkUserHostServerValidity(msgto, percentPos, atPos);
	}
	else if (percentPos != std::string::npos)
	{ // <username>%<hostname>
		info = _checkUserHostValidity(msgto, percentPos);
	}
	else if (exclamPos != std::string::npos && atPos != std::string::npos)
	{ // <nickname>!<username>@<hostname>
		info = _checkNickUserHostValidity(msgto, exclamPos, atPos);
	}
	else if (atPos != std::string::npos)
	{ // <username>@<servername>
		info = _checkUserServerValidity(msgto, atPos);
	}
	else
	{ // <nickname>
		isNicknameInServer(msgto);
		info.nickname = msgto;
		info.type = "nickname";
	}
	return info;
}

/*
	<username>%<hostname>@<servername>
*/
MsgToInfo CommandPrivmsg::_checkUserHostServerValidity(const std::string& msgto, size_t percentPos, size_t atPos)
{
	MsgToInfo info;

	std::string username = msgto.substr(0, percentPos);
	std::string hostname = msgto.substr(percentPos + 1, atPos - percentPos - 1);
	std::string triedServername = msgto.substr(atPos + 1);

	std::vector<std::string> nicknames = _server->getNicknameFromUsername(username);
	if (nicknames.empty())
	{
		_serverReply = ErrorReplies::errNoSuchNick(_client->getNickname(), _params[0]);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NoNicknameAssociatedException();
	}
	else if (nicknames.size() > 1)
	{
		std::stringstream ss;
		ss << nicknames.size();
		_serverReply = ErrorReplies::errTooManyTargets(_client->getNickname(), _params[0], ss.str(), "No message delivered");
		_server->handleNewMsgToClient(_serverReply, _client);
		throw TooManyTargetsException();
	}
	else
		info.nickname = nicknames[0];


	Client *target = _server->getClient(info.nickname);
	if (hostname != target->getHostname())
	{
		_serverReply = ":" + serverName + " ERROR: PRIVMSG The hostname " + hostname + " does not match the one associated with nickname " + info.nickname + "\r\n";
		_server->handleNewMsgToClient(_serverReply, _client);
		throw HostnameDoesNotMatchException();
	}

	if (triedServername != serverName)
	{
		_serverReply = ":" + serverName + " ERROR: PRIVMSG The servername " + triedServername + " does not match '" + serverName + "' \r\n";
		_server->handleNewMsgToClient(_serverReply, _client);
		throw ServernameDoesNotMatchException();
	}
	info.type = "usernameHostServer";
	return info;
}

/*
	<username>%<hostname>
*/
MsgToInfo CommandPrivmsg::_checkUserHostValidity(const std::string& msgto, size_t percentPos)
{
	MsgToInfo info;

	std::string username = msgto.substr(0, percentPos);
	std::string hostname = msgto.substr(percentPos + 1);

	std::vector<std::string> nicknames = _server->getNicknameFromUsername(username);
	if (nicknames.empty())
	{
		_serverReply = ErrorReplies::errNoSuchNick(_client->getNickname(), _params[0]);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NoNicknameAssociatedException();
	}
	else if (nicknames.size() > 1)
	{
		std::stringstream ss;
		ss << nicknames.size();
		_serverReply = ErrorReplies::errTooManyTargets(_client->getNickname(), _params[0], ss.str(), "No message delivered");
		_server->handleNewMsgToClient(_serverReply, _client);
		throw TooManyTargetsException();
	}
	else
		info.nickname = nicknames[0];

	Client *target = _server->getClient(info.nickname);
	if (hostname != target->getHostname())
	{
		_serverReply = ":" + serverName + " ERROR: PRIVMSG The hostname " + hostname + " does not match the one associated with nickname " + info.nickname + "\r\n";
		_server->handleNewMsgToClient(_serverReply, _client);
		throw HostnameDoesNotMatchException();
	}

	info.type = "usernameHost";
	return info;
}

/*
	<username>@<servername>
*/
MsgToInfo CommandPrivmsg::_checkUserServerValidity(const std::string& msgto, size_t atPos)
{
	MsgToInfo info;

	std::string username = msgto.substr(0, atPos);
	std::string triedServername = msgto.substr(atPos + 1);

	std::vector<std::string> nicknames = _server->getNicknameFromUsername(username);
	if (nicknames.empty())
	{
		_serverReply = ErrorReplies::errNoSuchNick(_client->getNickname(), _params[0]);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NoNicknameAssociatedException();
	}
	else if (nicknames.size() > 1)
	{
		std::stringstream ss;
		ss << nicknames.size();
		_serverReply = ErrorReplies::errTooManyTargets(_client->getNickname(), _params[0], ss.str(), "No message delivered");
		_server->handleNewMsgToClient(_serverReply, _client);
		throw TooManyTargetsException();
	}
	else
		info.nickname = nicknames[0];

	if (triedServername != serverName)
	{
		_serverReply = ":" + serverName + " ERROR: PRIVMSG The servername " + triedServername + " does not match \"ircserv\".\r\n";
		_server->handleNewMsgToClient(_serverReply, _client);
		throw ServernameDoesNotMatchException();
	}

	info.type = "usernameServername";
	return info;
}

/*
	<nickname>!<username>@<hostname>
*/
MsgToInfo CommandPrivmsg::_checkNickUserHostValidity(const std::string& msgto, size_t exclamPos, size_t atPos)
{
	MsgToInfo info;

	std::string nickname = msgto.substr(0, exclamPos);
	std::string username = msgto.substr(exclamPos + 1, atPos - exclamPos - 1);
	std::string hostname = msgto.substr(atPos + 1);

	isNicknameInServer(nickname);

	Client *target = _server->getClient(nickname);
	if (target == NULL)
	{
		_serverReply = ErrorReplies::errNoSuchNick(_client->getNickname(), nickname);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NoSuchNickException();
	}
	if (username == target->getUsername() && hostname == target->getHostname())
		info.nickname = nickname;
	else if (username != target->getUsername())
	{
		_serverReply = ":" + serverName + " ERROR: PRIVMSG There's no nickname being associated with username " + username + "\r\n";
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NoNicknameAssociatedException();
	}
	else if (hostname != target->getHostname())
	{
		_serverReply = ":" + serverName + " ERROR: PRIVMSG The hostname " + hostname + " does not match the one associated with nickname " + nickname + "\r\n";
		_server->handleNewMsgToClient(_serverReply, _client);
		throw HostnameDoesNotMatchException();
	}
	info.type = "nickUserHost";
	return info;
}

std::string CommandPrivmsg::_formatMsgToSend(const std::string& rawMsgToSend)
{
	std::string formatedMsgToSend = rawMsgToSend;
	if (formatedMsgToSend.find(' ') != std::string::npos)
		formatedMsgToSend = ":" + formatedMsgToSend;
	if (formatedMsgToSend.length() >= 2)
	{
		std::string lastTwoChars = formatedMsgToSend.substr(formatedMsgToSend.length() - 2, 2);
		if (lastTwoChars != "\r\n")
			formatedMsgToSend += "\r\n";
	}
	else
		formatedMsgToSend += "\r\n";
	return formatedMsgToSend;
}

/* ==================================================================================
    * Execution
================================================================================== */

void CommandPrivmsg::execute(void)
{
	if (_msgToInfo.type == "channel")
	{
		// Looking for a channel...
		Channel *destChan = _server->getChannel(_msgToInfo.channelName);
		if (destChan == NULL)
		{ // No corresponding channel found...
			_serverReply = ErrorReplies::errNoSuchNick(_client->getNickname(), _msgToInfo.channelName);
			_server->handleNewMsgToClient(_serverReply, _client);
			throw NoSuchNickException();
		}
		// Channel does exist...
		if (!destChan->isMember(_client->getNickname())) // If sender not in channel...
		{
			_serverReply = ErrorReplies::errCannotSendToChan(_client->getNickname(), _msgToInfo.channelName);
			_server->handleNewMsgToClient(_serverReply, _client);
			throw CannotSendToChanException();
		}
		std::vector<std::string> chanClientsNames = destChan->getClients();
		std::vector<std::string>::iterator it = chanClientsNames.begin();
		for ( ; it != chanClientsNames.end(); ++it)
		{
			if (*it == _client->getNickname())
				continue ;
			Client *recipient = _server->getClient(*it);
			_serverReply = ":" + _client->getHostmask() + " PRIVMSG " + _msgToInfo.channelName + " " + _msgToSend;
			_server->handleNewMsgToClient(_serverReply, recipient);
		}
	}
	else // If type is not "channel", there must be a nickname provided (see above parsing)
	{
		Client *recipient = _server->getClient(_msgToInfo.nickname);
		_serverReply = ":" + _client->getHostmask() + " PRIVMSG " + _msgToInfo.nickname + " " + _msgToSend;
		std::cout << "_serverReply = " << Utils::formatMessage(_serverReply) << std::endl;
		_server->handleNewMsgToClient(_serverReply, recipient);
	}
	return ;
}

/* ==================================================================================
    * Exceptions
================================================================================== */

const char* CommandPrivmsg::NoSuchNickException::what() const throw()
{
	return "Nickname not found on server.";
}

const char* CommandPrivmsg::NoRecipientException::what() const throw()
{
	return "No recipient provided.";
}

const char* CommandPrivmsg::NoTextToSendException::what() const throw()
{
	return "You should provide a text to be sent.";
}

const char* CommandPrivmsg::CannotSendToChanException::what() const throw()
{
	return "Cannot send to chan.";
}

const char* CommandPrivmsg::TooManyTargetsException::what() const throw()
{
	return "Too many targets.";
}

/* ************************************************************************** */

const char* CommandPrivmsg::HostnameDoesNotMatchException::what() const throw()
{
	return "Hostname does not match the provided one.";
}

const char* CommandPrivmsg::ServernameDoesNotMatchException::what() const throw()
{
	return "Servername does not match our real servername.";
}

const char* CommandPrivmsg::NoNicknameAssociatedException::what() const throw()
{
	return "No nickname associated with the provided username.";
}
