/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmourtia <vmourtia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 20:44:17 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/20 12:30:42 by vmourtia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "CommandPass.hpp"
#include "CommandNick.hpp"
#include "CommandUser.hpp"
#include "CommandMode.hpp"
#include "CommandJoin.hpp"
#include "CommandPrivmsg.hpp"
#include "CommandInvite.hpp"
#include "CommandTopic.hpp"
#include "CommandKick.hpp"
#include "CommandPing.hpp"
#include "CommandPart.hpp"
#include "CommandQuit.hpp"

/*
	RFC 2812: 2.3

	IRC messages are always lines of characters terminated with a CR-LF
	(Carriage Return - Line Feed) pair, and these messages SHALL NOT
	exceed 512 characters in length, counting all characters including
	the trailing CR-LF.
*/

/* ==================================================================================
    * Constructor
================================================================================== */

Parser::Parser(Server *server)
	: _server(server),
	_serverReply(""),
	_messages()
{
}


/* ==================================================================================
    * Split the client request
================================================================================== */

/*
	How it works:

	request = "PASS vaval\r\nNICK\r\nJOIN #channel\r\n"
	_messages = [ "PASS vaval\r\n", "NICK\r\n", "JOIN #channel\r\n" ]

	request = "PASS vaval\r\nNICK vm"
	_messages = [ "PASS vaval\r\n", "NICK vm" ]
*/
void Parser::_splitMessages(const std::string &request, Client *client)
{
	std::string::size_type startPos = 0;
	std::string::size_type endPos;

	while ((endPos = request.find("\r\n", startPos)) != std::string::npos)
	{
		// Add the message to _messages INCLUDED "\r\n"
		std::string message = request.substr(startPos, endPos - startPos + 2);
		if (message.size() > 512)
		{
			std::string rplMsgTooLong = ":" + serverName + " ERROR: Message exceeds 512 characters\r\n";
			_server->handleNewMsgToClient(rplMsgTooLong, client);
			std::cerr << "ERROR: Message exceeds 512 characters" << std::endl;
			continue ;
		}
		_messages.push_back(message);
		startPos = endPos + 2;
		if (_server->hasShutDown() == true)
			return ;
	}

	// Add the remaining chars of the request if it doesn't end by "\r\n"
	if (startPos < request.length())
		_messages.push_back(request.substr(startPos));

	// VMOURTIA: custom DEBUG messages need to be deleted.
	// std::cout << "\n" << BRIGHT_GREEN << "_splitMessages(\"" << Utils::formatMessage(request) << "\") = " << std::endl;
	// for (std::vector<std::string>::iterator it = _messages.begin(); it != _messages.end(); ++it) {
	// 	std::cout << "\t- \"" << Utils::formatMessage(*it) << "\"" << std::endl;
	// }
	// std::cout << RESET << "\n" << std::endl;

}

/* ==================================================================================
    * Tokenizer
================================================================================== */
/*
	Original message = USER oce 0 * :oceane razafy\r\n
	- Token[0] = USER
	- Token[1] = oce
	- Token[2] = 0
	- Token[3] = *
	- Token[4] = oceane razafy
*/
std::vector<std::string> Parser::_tokenizeMessage(std::string message, Client *client) {
	if (message.empty())
		throw EmptyMessageException();

	Logger::logValue(GREEN, "\n[PARSER] message in _tokenizeMessage", message);

	std::vector<std::string> tokens;
	std::string::size_type startPos = 0;
	std::string::size_type spacePos;
	bool isTrailing = false;

	// Remove "\r\n" at the end if it does exist
	if (message.length() >= 2 && message.substr(message.length() - 2) == "\r\n")
		message.erase(message.length() - 2);

	int i = 0;
	while (startPos < message.size() && !isTrailing)
	{
		if (_server->hasShutDown() == true)
			break;
		spacePos = message.find(' ', startPos);

		if (i == 0 && spacePos != std::string::npos)
		{
			if (message[0] == ':'
				&& message.substr(1, spacePos - 1) != client->getNickname())
			{
				throw InvalidPrefixException();
			}
			else if (message[0] == ':'
					&& message.substr(1, spacePos - 1) == client->getNickname())
			{
				startPos = spacePos + 1;
			}
			else
			{
				tokens.push_back(message.substr(0, spacePos));
				startPos = spacePos + 1;
			}
			i++;
			continue;
		}

		if (message[startPos] == ':')
		{
			tokens.push_back(message.substr(startPos + 1));
			isTrailing = true;
			break;
		}
		else if (spacePos != std::string::npos)
		{
			tokens.push_back(message.substr(startPos, spacePos - startPos));
			startPos = spacePos + 1;
			i++;
		}
		else
		{
			// Last token before the end of the line
			tokens.push_back(message.substr(startPos));
			break;
		}
	}

	// VMOURTIA: custom DEBUG messages need to be deleted.
	// std::cout << "\n" << BRIGHT_BLUE << "tokens = " << std::endl;
	// for (size_t i = 0; i < tokens.size(); ++i) {
	// 	std::cout << "\t- [" << i << "] = \"" << Utils::formatMessage(tokens[i]) << "\"" << std::endl;
	// }
	// std::cout << RESET << "\n" << std::endl;

	return tokens;
}

std::vector<std::string> Parser::getMessages(void) const
{
	return _messages;
}

/* ==================================================================================
    * Parsing the request
================================================================================== */

/*
	RFC 2812: 
	
	2.3 Messages:
	Each IRC message may consist of up to three main parts: the prefix
	(OPTIONAL), the command, and the command parameters (maximum of
	fifteen (15)).
*/
void Parser::parseRequest(Client *client)
{
	std::cout << std::endl;

	if (client->getRequest().find("\r\n") == std::string::npos)
		return ;


	_splitMessages(client->getRequest(), client);

	// Logger::logMessages(BRIGHT_GREEN, _messages);

	// Handle all the complete messages (those finishing by \r\n)
	for (size_t i = 0; i < _messages.size() - 1; ++i)
	{
		if (_server->hasShutDown() == true)
			return ;
		try
		{
			_processMessage(client, _messages[i]);
			if (client->getHasQuit() == true)
			{
				_messages.clear();
				return ;
			}
		}
		catch (const Parser::InvalidPrefixException &e)
		{
			std::string rplInvalidPrefix = ":" + serverName + " ERROR: invalid prefix: does not match with client nickname\r\n";
			_server->handleNewMsgToClient(rplInvalidPrefix, client);
			std::cerr << "ERROR: invalid prefix: does not match with client nickname" << std::endl;

		}
		
	}

	std::string lastMessage = _messages[_messages.size() - 1];
	// Is the last message complete ?
	
	if (lastMessage.find("\r\n") != std::string::npos)
	{
		try
		{
			client->storeRequestData("");
			_processMessage(client, lastMessage);
			if (client->getHasQuit() == true)
			{
				_messages.clear();
				return ;
			}
		}
		catch (const Parser::InvalidPrefixException &e)
		{
			std::string rplInvalidPrefix = ":" + serverName + " ERROR: invalid prefix: does not match with client nickname\r\n";
			_server->handleNewMsgToClient(rplInvalidPrefix, client);
			std::cerr << "ERROR: invalid prefix: does not match with client nickname" << std::endl;
		}
	}
	else
	{
		// The last message isn't complete so we keep looping
		client->storeRequestData(lastMessage);
	}
	_messages.clear();
}

std::vector<std::string> Parser::_getParamsFromTokens(std::vector<std::string> tokens) const
{
	Logger::logVector(GREEN, "[PARSER] tokens", tokens);
	if (tokens.begin() + 1 == tokens.end())
		return std::vector<std::string>();
	else
		return std::vector<std::string>(tokens.begin() + 1, tokens.end());
}

/* ==================================================================================
    * Processing each client message (each command line)
================================================================================== */

void Parser::_processMessage(Client *client, std::string message)
{
	std::vector<std::string> tokens = _tokenizeMessage(message, client);

	if (tokens.empty() || _server->hasShutDown() == true)
		return ;
	// Logger::logTokens(BRIGHT_GREEN, tokens, message);
	
	std::vector<std::string> params = _getParamsFromTokens(tokens);
	std::string command = tokens[0]; // VMOURTIA: UPPER REMOVED
	if (command == "PASS")
	{
		try
		{
			CommandPass passCmd(params, client, _server);
			passCmd.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: PASS command - " << e.what() << std::endl;
		}
	}
	else if (command == "NICK")
	{
		try
		{
			CommandNick nickCmd(params, client, _server);
			nickCmd.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: NICK command - " << e.what() << std::endl;
		}
	}
	else if (command == "USER")
	{
		try
		{
			CommandUser userCmd(params, client, _server);
			userCmd.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: USER command - " << e.what() << std::endl;
		}
	}
	else if (command == "MODE")
	{
		try
		{
			CommandMode modeCmd(params, client, _server);
			modeCmd.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: MODE command - " << e.what() << std::endl;
		}
	}
	else if (command == "JOIN")
	{
		try
		{
			CommandJoin joinCmd(params, client, _server);
			joinCmd.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: JOIN command - " << e.what() << std::endl;
		}
	}
	else if (command == "PRIVMSG")
	{
		try
		{
			CommandPrivmsg privmsgCmd(params, client, _server);
			privmsgCmd.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: PRIVMSG command - " << e.what() << std::endl;
		}
	}
	else if (command == "INVITE")
	{
		try
		{
			CommandInvite inviteCmd(params, client, _server);
			inviteCmd.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: INVITE command - " << e.what() << std::endl;
		}
	}
	else if (command == "TOPIC")
	{
		try
		{
			CommandTopic topicCmd(params, client, _server);
			topicCmd.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: TOPIC command - " << e.what() << std::endl;
		}
	}
	else if (command == "KICK")
	{
		try
		{
			CommandKick kickCmd(params, client, _server);
			kickCmd.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: KICK command - " << e.what() << std::endl;
		}
	}
	else if (command == "PING")
	{
		try
		{
			CommandPing pingCmd(params, client, _server);
			pingCmd.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: PING command - " << e.what() << std::endl;
		}
	}
	else if (command == "PART")
	{
		try
		{
			CommandPart partCmd(params, client, _server);
			partCmd.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: PART command - " << e.what() << std::endl;
		}
	}
	else if (command == "QUIT")
	{
		try
		{
			CommandQuit partQuit(params, client, _server);
			partQuit.execute();
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: QUIT command - " << e.what() << std::endl;
		}
	}
	else
	{
		if (client->getIsRegistered() == false)
		{
			_serverReply = ErrorReplies::errNotRegistered(client->getNickname());
			_server->handleNewMsgToClient(_serverReply, client);
			std::cerr << "ERROR: Client not registered" << std::endl;
		}
		else
		{
			_serverReply = ErrorReplies::errUnknownCommand(client->getNickname(), command);
			_server->handleNewMsgToClient(_serverReply, client);
			std::cerr << "ERROR: Unknown command" << std::endl;
		}	
	}
}

/* ==================================================================================
    * Exceptions
================================================================================== */

/*
	RFC 2812: 2.3.1

	Empty messages are silently ignored,
	which permits use of the sequence CR-LF between messages without
	extra problems.
*/
const char* Parser::EmptyMessageException::what() const throw()
{
	return "Empty message: need to be silently ignored.";
}

const char* Parser::InvalidPrefixException::what() const throw()
{
	return "Nickname not corresponding to the client's one.";
}