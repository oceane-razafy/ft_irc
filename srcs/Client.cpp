/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 00:43:30 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/12 17:55:54 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* ==================================================================================
    * Constructor
================================================================================== */

Client::Client(pollfd pollfd)
	: _pollfd(pollfd),
	_fd(_pollfd.fd),
	_request(""),
	_nickname(""),
	_username(""),
	_hostname(""),
	_realname(""),
	_isRegistered(false),
	_PASSRequestDone(false),
	_NICKRequestDone(false),
	_hasQuit(false),
	_channels(),
	_pendingServerMsgs()
{
	
}

/* ==================================================================================
    * Getters
================================================================================== */

pollfd Client::getPollfd(void) const
{
	return (_pollfd);
}

int	Client::getFd(void) const
{
	return (_fd);
}

const std::string	Client::getRequest(void)
{
	return (_request);
}

std::string	Client::getNickname(void) const
{
	return (_nickname);
}

std::string	Client::getUsername(void) const
{
	return (_username);
}

std::string	Client::getHostname(void) const
{
	return (_hostname);
}

std::string	Client::getRealname(void) const
{
	return (_realname);
}

std::string	Client::getHostmask(void)
{
	return (_nickname + "!" + _username + "@" + _hostname);
}

bool	Client::getIsRegistered(void) const
{
	return(_isRegistered);
}

bool	Client::getPASSRequestDone(void) const
{
	return (_PASSRequestDone);
}

bool	Client::getNICKRequestDone(void) const
{
	return (_NICKRequestDone);
}

bool	Client::getHasQuit(void) const
{
	return (_hasQuit);
}

std::queue<std::string>		&Client::getPendingServerMsgs(void) // reference here, server will modify it, depending on which msg was sent successfully
{
	return (_pendingServerMsgs);
}

std::vector<std::string>	Client::getChannels(void) const
{
	return (_channels);
}

/* ==================================================================================
    * Setters
================================================================================== */

void	Client::setNickname(const std::string &nickname)
{
	_nickname = nickname;
}

void	Client::setUsername(const std::string &username)
{
	_username = username;
}

void	Client::setHostname(const std::string &hostname)
{
	_hostname = hostname;
}

void	Client::setRealname(const std::string &realname)
{
	_realname = realname;
}

void	Client::setIsRegistered(bool isRegistered)
{
	_isRegistered = isRegistered;
}

void	Client::setPASSRequestDone(bool PASSRequestDone)
{
	_PASSRequestDone = PASSRequestDone;
}

void	Client::setNICKRequestDone(bool NICKRequestDone)
{
	_NICKRequestDone = NICKRequestDone;
}

void	Client::setHasQuit(bool hasQuit)
{
	_hasQuit = hasQuit;
}

/* ==================================================================================
    * Store client request
================================================================================== */

void Client::storeRequestData(const std::string &data)
{
	_request = data;
}

void Client::updateRequest(const std::string &data)
{
	_request += data;
}

/* ==================================================================================
    * Functions to handle client channels
	NOTE: Channel names are case insensitive
================================================================================== */

// Add a channel to the client channels vector
void	Client::addChannel(std::string newChannel)
{
	newChannel = Utils::toLowerChannelName(newChannel);
    if (std::find(_channels.begin(), _channels.end(), newChannel) == _channels.end())
		_channels.push_back(newChannel);
}

// Remove a channel to the client channels vector
void	Client::removeChannel(std::string channelName)
{
	channelName = Utils::toLowerChannelName(channelName);
	std::vector<std::string>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (*it == channelName)
		{
			_channels.erase(it);
			break ;
		}
	}
}

// Check if the client is part of the channel
bool	Client::isPartOfThisChannel(std::string channelName)
{
	channelName = Utils::toLowerChannelName(channelName);
	std::vector<std::string>::iterator it;
	for (it = _channels.begin(); it != _channels.end(); it++)
	{
		if (*it == channelName)
			return (true);
	}
	return (false);
}

/* ==================================================================================
    * Add a new pending server message
================================================================================== */

// Add a new server message in the client pending server messages
void	Client::addNewServerMsg(const std::string &msg)
{
	_pendingServerMsgs.push(msg);
}
