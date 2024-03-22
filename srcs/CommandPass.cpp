/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandPass.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/25 18:57:52 by valentin          #+#    #+#             */
/*   Updated: 2024/03/08 21:04:55 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandPass.hpp"
#include "Configuration.hpp"

/*
	RFC 2812

	Command: PASS
   	Parameters: <password>

	The PASS command is used to set a 'connection password'.
	
	Numeric Replies:

        ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED
*/

/* ==================================================================================
    * Constructor
================================================================================== */

CommandPass::CommandPass(std::vector<std::string> params, Client * client, Server * server)
	: ACommand(params, client, server)
{
	Logger::logVector(PURPLE, "[PASS] _params", _params);

	std::string rplPassAlreadyProvided = ":" + serverName + " :Password already provided.\r\n";

	// Client is already registered (ERR_ALREADYREGISTRED)
	if (_client->getIsRegistered())
	{
		_serverReply = ErrorReplies::errAlreadyRegistred(_client->getNickname());
		_server->handleNewMsgToClient(_serverReply, _client);
		throw AlreadyRegisteredException();
	}
	
	// Client not registered but password already provided
	if (_client->getPASSRequestDone()) 
	{
		_server->handleNewMsgToClient(rplPassAlreadyProvided, _client);
		throw PassAlreadyProvidedException();
	}

	// Check the number of parameters provided (ERR_NEEDMOREPARAMS)
	checkParamsNb("PASS", 1, 1);

	_triedPassword = _params[0];
}

/* ==================================================================================
    * Execution
================================================================================== */

void CommandPass::execute(void)
{
	if (_triedPassword == _server->getConfig().getPassword())
		_client->setPASSRequestDone(true);
	// Wrong password provided (ERR_PASSWDMISMATCH)
	else
	{
		_serverReply = ErrorReplies::errPasswdMismatch(_client->getNickname());
		_server->handleNewMsgToClient(_serverReply, _client);
	}
}

/* ==================================================================================
    * Exceptions
================================================================================== */

const char* CommandPass::AlreadyRegisteredException::what() const throw()
{
	return "Already registered.";
}

const char* CommandPass::PassAlreadyProvidedException::what() const throw()
{
	return "Password already provided.";
}
