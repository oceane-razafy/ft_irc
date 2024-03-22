/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommand.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: valentin <valentin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 01:38:50 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/12 21:56:30 by valentin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ACommand.hpp"

/* ==================================================================================
    * Constructor
================================================================================== */

ACommand::ACommand(std::vector<std::string> params, Client *client, Server *server)
	: _params(params),
	_client(client),
	_server(server),
	_serverReply("")
{
}

/* ==================================================================================
    * Helpers to execute each concrete command class
================================================================================== */

// If the request has a parameter list, split it and put all elements in a vector
std::vector<std::string> ACommand::splitParametersList(const std::string& parametersList)
{
	std::vector<std::string> elements;
	std::istringstream iss(parametersList);
	std::string element;

	// Put each parameter in the vector elements
	while (getline(iss, element, ','))
		elements.push_back(element);

	// Return the vector of parameters
	return (elements);
}

// Check if the command has enough/too many parameters
void	ACommand::checkParamsNb(const std::string &command, long unsigned int min, long unsigned int max)
{
	if (_params.size() < min)
	{
		_serverReply = ErrorReplies::errNeedMoreParams(_client->getNickname(), command);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NeedMoreParamsException();
	}
		
	if (_params.size() > max)
	{
		_serverReply = ErrorReplies::errTooManyParams(_client->getNickname(), command);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw TooManyParamsException();
	}
}

// Check if client who issued the command is registered
void	ACommand::checkIsRegistered(void)
{
	if (_client->getIsRegistered() == false)
	{
		_serverReply = ErrorReplies::errNotRegistered(_client->getNickname());
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NotRegisteredException();
	}
}

// Check if channel provided for the command exists, and if yes, get it
Channel *ACommand::getChannel(std::string channelName)
{
	Channel *channel;
	
	channelName = Utils::toLowerChannelName(channelName);
	channel = _server->getChannel(channelName);
	if (channel == NULL)
	{
		_serverReply = ErrorReplies::errNoSuchChannel(_client->getNickname(), channelName);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NoSuchChannelException();
	}
	return (channel);
}

// Check if nickname provided exists in the server
void	ACommand::isNicknameInServer(const std::string &nickname)
{
	Client *client = _server->getClient(nickname);
	if (client != NULL)
		return ;
	Logger::log(RED, "AKI");
	_serverReply = ErrorReplies::errNoSuchNick(_client->getNickname(), nickname);
	_server->handleNewMsgToClient(_serverReply, _client);
	throw NoSuchNickException();
	Logger::log(RED, "THROW");
}

/*
	==> Check if client or target is in the channel
	
		441    ERR_USERNOTINCHANNEL
              "<nick> <channel> :They aren't on that channel"

         	- Returned by the server to indicate that the target
          	user of the command is not on the given channel.

       	442    ERR_NOTONCHANNEL
              "<channel> :You're not on that channel"

         	- Returned by the server whenever a client tries to
          	perform a channel affecting command for which the
           	client isn't a member.
	
	=> EXAMPLES of isNickanmeInChannel
	isNicknameInChannel("john", "#tennis", true) -> check if john who issued the command is in the channel
	isNicknameInChannel("chris", "#tennis", false) -> check if chris who is th target of the command is in the channel
*/
void	ACommand::isNicknameInChannel(const std::string &nickname, std::string channelName, bool forClientWhoAsked)
{
	channelName = Utils::toLowerChannelName(channelName);

	// Get the channel
	Channel *channel = _server->getChannel(channelName);
	
	// If the nickname is the one of the client who issued the command
	if (channel == NULL || (channel->isMember(nickname) == false && forClientWhoAsked == true))
	{
		_serverReply = ErrorReplies::errNotOnChannel(_client->getNickname(), channelName);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw ClientNotInChannelException();
	}
	
	// If the nickname is the the one of the target of the command
	if (channel == NULL || (channel->isMember(nickname) == false && forClientWhoAsked == false))
	{
		_serverReply = ErrorReplies::errUserNotInChannel(_client->getNickname(), nickname, channelName);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw TargetNotInChannelException();
	}
}

/* ==================================================================================
    * Exceptions
================================================================================== */

const char* ACommand::NeedMoreParamsException::what() const throw()
{
	return ("Need more params.");
}

const char* ACommand::NotRegisteredException::what() const throw()
{
	return ("Client not registered.");
}

const char* ACommand::TooManyParamsException::what() const throw()
{
	return ("Too many params.");
}

const char* ACommand::NoSuchNickException::what() const throw()
{
	return ("No such nickname.");
}

const char* ACommand::NoSuchChannelException::what() const throw()
{
	return ("No such channel.");
}

const char* ACommand::ClientNotInChannelException::what() const throw()
{
	return ("Client not in channel");
}

const char* ACommand::TargetNotInChannelException::what() const throw()
{
	return ("Target not in the channel");
}

const char* ACommand::NotOperatorException::what() const throw()
{
	return ("Client not an operator.");
}
