/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandUser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:50:36 by valentin          #+#    #+#             */
/*   Updated: 2024/03/19 13:27:05 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandUser.hpp"

/*
	RFC 2812

	Command: USER
   	Parameters: <user> <mode> <unused> <realname>

	The USER command is used at the beginning of connection to specify
   	the username, hostname and realname of a new user.
	
	Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED
*/

CommandUser::CommandUser(std::vector<std::string> params, Client *client, Server *server)
	: ACommand(params, client, server)
{
	Logger::logVector(PURPLE, "[USER] _params", _params);

	if (!_client->getPASSRequestDone() && !_client->getNICKRequestDone())
	{
		std::string rplNoPwdNoNickProvided = ":" + serverName + " :Enter the server password and a nickname before executing the USER command.\r\n";
		_server->handleNewMsgToClient(rplNoPwdNoNickProvided, _client);
		throw NoPwdNoNickException();
	}
	
	if (_client->getPASSRequestDone() && !_client->getNICKRequestDone())
	{
		std::string rplNoNickProvided = ":" + serverName + " :Enter a nickname before executing the USER command.\r\n";
		_server->handleNewMsgToClient(rplNoNickProvided, _client);
		throw NoNickException();
	}

	// Check the number of parameters provided (ERR_NEEDMOREPARAMS)
	checkParamsNb("USER", 4, 4);

	if (_client->getIsRegistered())
	{
		_serverReply = ErrorReplies::errAlreadyRegistred(_client->getNickname());
		_server->handleNewMsgToClient(_serverReply, _client);
		throw AlreadyRegisteredException();
	}

	if (_isValidUsername(_params[0]))
		_username = _params[0];
	else
	{
		_serverReply = ":" + serverName + " ERROR: USER Invalid username\r\n";
		_server->handleNewMsgToClient(_serverReply, _client);
		throw InvalidUsernameException();
	}

	_hostname = _getClientIpAdrr(_client->getFd());
	_realname = _params[3];
}

void CommandUser::execute(void)
{
	_client->setUsername(_username);
	_client->setHostname(_hostname);
	_client->setRealname(_realname);
	_client->setIsRegistered(true);

	_serverReply = CommandResponses::rplWelcome(_client->getNickname(), _username, _hostname);
	_server->handleNewMsgToClient(_serverReply, _client);

	_serverReply = CommandResponses::rplYourHost(_client->getNickname());
	_server->handleNewMsgToClient(_serverReply, _client);
	
	_serverReply = CommandResponses::rplCreated(_client->getNickname());
	_server->handleNewMsgToClient(_serverReply, _client);

	_serverReply = CommandResponses::rplMyInfo(_client->getNickname());
	_server->handleNewMsgToClient(_serverReply, _client);
}

// VMOURTIA: We need to discuss the choices made when an error occured !
std::string CommandUser::_getClientIpAdrr(int clientFd)
{
	struct sockaddr_in addr;
	socklen_t size_addr = sizeof(addr);
	if (getsockname(clientFd, (struct sockaddr *)&addr, &size_addr) == -1)
		return _params[2];
	char* clientIP = inet_ntoa(addr.sin_addr);
	if (clientIP == NULL) // Invalid address provided
		return _params[2];
	return clientIP;
}

bool CommandUser::_isValidUsername(const std::string& username)
{
	if (username.empty())
	{
		return false;
	}

	for (size_t i = 0; i < username.length(); ++i)
	{
		unsigned char c = username[i];

		if (c == 0x00 || c == '\r' || c == '\n' || c == ' ' || c == '@')
			return false;

		if (!((c >= 0x01 && c <= 0x09) // SOH -> HT
				|| (c >= 0x0B && c <= 0x0C) // \v and \f
				|| (c >= 0x0E && c <= 0x1F) // Shift out -> Unit separator
				|| (c >= 0x21 && c <= 0x3F) // ! -> ?
				|| (c >= 0x41))) // A -> and further until FF (255)
		{
			return false;
		}
	}

	return true;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*                                Errors                                      */
/*                                                                            */
/*                                                                            */
/* ************************************************************************** */

const char* CommandUser::NoPwdNoNickException::what() const throw()
{
	return "Need a password and a nickname before user infos.";
}

const char* CommandUser::NoNickException::what() const throw()
{
	return "Need a nickname before user infos.";
}

const char* CommandUser::AlreadyRegisteredException::what() const throw()
{
	return "Already registered.";
}

const char* CommandUser::InvalidUsernameException::what() const throw()
{
	return "Invalid username provided.";
}
