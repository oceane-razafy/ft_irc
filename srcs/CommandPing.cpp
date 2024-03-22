/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandPing.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 22:43:09 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/08 20:58:40 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandPing.hpp"

// Custom PING command execution to handle ping messages from IRSSI, our reference client

/* ==================================================================================
    * Constructor
================================================================================== */

CommandPing::CommandPing(std::vector<std::string> params, Client *client, Server *server)
	: ACommand(params, client, server)
{
	Logger::logVector(CYAN, "[PING] _params", _params);

	// Check if client who issued the command is registered
	checkIsRegistered();

	/*
		One type of PING message is accepted "PING <server name>" as there is only one server
		and that's how IRSSI 1.2.3 is working
	
		- Check the number of parameters provided (ERR_NEEDMOREPARAMS)
		- Check if first param equals this server name
	*/
	checkParamsNb("PING", 1, 1);
	if (_params[0] != serverName)
	{
		_serverReply = ErrorReplies::errNoSuchServer(_client->getNickname(), _params[0]);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NoSuchServerException();
	}
}

/* ==================================================================================
    * Execution
================================================================================== */

void	CommandPing::execute(void)
{
	// Reply with a PONG message
	_serverReply = ":" + serverName + " PONG " + serverName + "\r\n";
	_server->handleNewMsgToClient(_serverReply, _client);
}

/* ==================================================================================
    * Exceptions
================================================================================== */

const char* CommandPing::NoSuchServerException::what() const throw()
{
	return ("No such server");
}