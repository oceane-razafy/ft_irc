/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 17:45:12 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/08 22:27:41 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include "Utils.hpp"

void	Logger::log(const std::string &color, const std::string &msg)
{
	std::cout << color << msg << RESET << std::endl;
}

void	Logger::logDebug(const std::string &color, const std::string &msg)
{
	std::cout << color << "DEBUG: " << msg << RESET << std::endl;
}

void	Logger::logInfo(const std::string &color, const std::string &msg)
{
	std::cout << color << "INFO: " << msg << RESET << std::endl;
}

// Server
void	Logger::logPollSet(std::vector<pollfd> pollfds)
{
	std::cout << LIGHT_CYAN << "[SERVER]: Current number of fds: " << pollfds.size() << std::endl;
	std::cout << "\tServer \t   --> fd: " << pollfds[0].fd << std::endl;
	for (unsigned int i = 1; i < pollfds.size(); i++)
		std::cout << "\tClient n°" << i << " --> fd: " << pollfds[i].fd << std::endl;
	std::cout << RESET << std::endl;
}

void	Logger::logBeforeParsing(const std::string &color, const std::string &buffer, const std::string &request)
{
	std::cout << color << std::endl;
	std::cout << "[SERVER] BEFORE _parser->parseRequest(client) :" << std::endl;
	std::cout << "\tbuffer       = \"" << Utils::formatMessage(buffer) << "\"" << std::endl;
	std::cout << "\tclient->getRequest() = \"" << Utils::formatMessage(request) << "\"" << RESET;
}

void	Logger::logAfterParsing(const std::string &color, const std::string &buffer, const std::string &request)
{
	std::cout << color << "[SERVER] AFTER _parser->parseRequest(client) :" << std::endl;
	std::cout << "\tbuffer       = \"" << Utils::formatMessage(buffer) << "\"" << std::endl;
	std::cout << "\tclient->getRequest() = \"" << Utils::formatMessage(request) << "\"" << RESET << std::endl;
	std::cout << std::endl;
}

// Parser
void	Logger::logMessages(const std::string &color, std::vector<std::string> messages)
{
	std::cout << color << "[PARSER] All messages retrieved from client->getRequest() :" << std::endl;
	for (size_t i = 0; i < messages.size(); ++i)
		std::cout << "\tMessage[" << i << "]: " << Utils::formatMessage(messages[i]) << std::endl;
	std::cout << RESET;
}

void	Logger::logTokens(const std::string &color, std::vector<std::string> tokens, const std::string &message)
{
	std::cout << color << "[PARSER] Original message = " << Utils::formatMessage(message) << std::endl;
	for (size_t j = 0; j < tokens.size(); ++j)
		std::cout << "\t- Token[" << j << "] = " << Utils::formatMessage(tokens[j]) << std::endl;
	std::cout << RESET;
}

// Mode
void	Logger::logParams(const std::string &color, std::vector<std::string> params)
{
	std::cout << color << "[MODE] At the end of _cleanModeCmd() | _params =" << std::endl;
	for (size_t i = 0; i < params.size(); ++i)
		std::cout << "\t- _params[" << i << "] = " << "'" << params[i] << "'" << std::endl;
	std::cout << RESET;
}

void	Logger::logModes(const std::string &color, std::vector<ModeInfo> modes)
{
	std::cout << color << "[MODE] In _parseModes | _modes =" << std::endl;
	for (size_t i = 0; i < modes.size(); ++i) 
		std::cout << "\t_modes[" << i << "] = " << "{ '" << modes[i].modeOption << "' , '" << modes[i].mode << "', '" << modes[i].optionalArg << "' }" << std::endl;
	std::cout << RESET;
}


























