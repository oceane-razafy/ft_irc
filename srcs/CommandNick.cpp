/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandNick.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmourtia <vmourtia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 17:08:22 by valentin          #+#    #+#             */
/*   Updated: 2024/03/20 16:17:52 by vmourtia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandNick.hpp"
#include "Utils.hpp"

/*
	RFC 2812

	Command: NICK
	Parameters: <nickname>

	NICK command is used to give user a nickname or change the existing one.
	
	Numeric Replies:

           ERR_NONICKNAMEGIVEN             					ERR_ERRONEUSNICKNAME
           ERR_NICKNAMEINUSE               					ERR_NICKCOLLISION (not in the subject)
           ERR_UNAVAILRESOURCE (not in the subject)         ERR_RESTRICTED (not in the subject)
*/

/* ==================================================================================
    * Constructor
================================================================================== */

CommandNick::CommandNick(std::vector<std::string> params, Client *client, Server *server)
	: ACommand(params, client, server)
{
	Logger::logVector(BLUE, "[NICK] _params", _params);

	if (!_client->getPASSRequestDone())
	{
		std::string rplNoPwdProvided = ":" + serverName + " :Enter the server password before anything else.\r\n";
		_server->handleNewMsgToClient(rplNoPwdProvided, _client);
		throw NoPasswordProvidedException();
	}
	
	if (_params.size() > 1)
	{
		_serverReply = ErrorReplies::errTooManyParams(_client->getNickname(), "NICK");
		_server->handleNewMsgToClient(_serverReply, _client);
		throw TooManyParamsException();
	}
	if (_params.empty()) // 431 ERR_NONICKNAMEGIVEN
	{
		_serverReply = ErrorReplies::errNoNicknameGiven(_client->getNickname());
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NoNicknameGivenException();
	}

	_newNickname = _params[0];

	if (!_isNicknameValid(_newNickname)) // 432 ERR_ERRONEUSNICKNAME
	{
		_serverReply = ErrorReplies::errErroneusNickname(_client->getNickname(), _newNickname);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw ErroneousNicknameException() ;
	}
	// RESPONSE: Throw error 433 | What if a user who has the nickname "vaval" executes "NICK vaval" ?
	if (_isNicknameAlreadyInUse(_newNickname)) // 433 ERR_NICKNAMEINUSE
	{
		_serverReply = ErrorReplies::errNicknameInUse(_client->getNickname(), _newNickname);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NicknameInUseException() ;
	}
}

// FIX nickname with '-'
bool CommandNick::_isNicknameValid(std::string nickname) const
{
	if (nickname.empty() || nickname.length() > 9)
	{
		return false;
	}
	char firstChar = nickname[0];
	if (!(std::isalpha(static_cast<unsigned char>(firstChar))
			|| Utils::isSpecialChar(firstChar)))
	{
		return false;
	}
	for (size_t i = 1; i < nickname.length(); ++i)
	{
		char c = nickname[i];
		if (!(std::isalpha(static_cast<unsigned char>(c))
				|| std::isdigit(static_cast<unsigned char>(c))
				|| Utils::isSpecialChar(c)
				|| c == '-'))
		{
			return false;
		}
	}
	return true;
}

bool CommandNick::_isNicknameAlreadyInUse(std::string nickname) const
{
	const std::map<int, Client> clients = _server->getClients();

	std::map<int, Client>::const_iterator it = clients.begin();
	for ( ; it != clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
			return true;
	}
	return false;
}

/* ==================================================================================
    * Execution
================================================================================== */

void CommandNick::execute(void)
{
	if (!_client->getNickname().empty())
	{
		_serverReply = ":" + _client->getHostmask() + " NICK " + _newNickname + "\r\n";
		_server->broadcastMsgToAllMembers(_serverReply);

		std::vector<std::string> channelNames = _client->getChannels();
		std::vector<std::string>::iterator it;
		for(it = channelNames.begin(); it != channelNames.end(); it++)
		{
			Channel *channel = _server->getChannel(*it);
			channel->changeThisNickname(_newNickname, _client->getNickname());
			// std::vector<std::string> &clientNames = channel->getClients();
			// std::vector<std::string>::iterator itClient;
			// itClient = std::find(clientNames.begin(), clientNames.end(), _client->getNickname());
			// *itClient = _newNickname;
		}
	}
	_client->setNickname(_newNickname);
	_client->setNICKRequestDone(true);

	
}

/* ==================================================================================
    * Exceptions
================================================================================== */

const char* CommandNick::NoPasswordProvidedException::what() const throw()
{
	return "No password provided, can't execute NICK command.";
}

const char* CommandNick::NoNicknameGivenException::what() const throw()
{
	return "No nickname given.";
}

const char* CommandNick::ErroneousNicknameException::what() const throw()
{
	return "Erroneous nickname.";
}

const char* CommandNick::NicknameInUseException::what() const throw()
{
	return "Nickname already in use.";
}
