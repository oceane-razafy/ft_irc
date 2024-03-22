/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorReplies.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 20:20:24 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/20 14:57:19 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorReplies.hpp"

/*
	RFC 2812
	
	5.2 Error Replies
	
    Error replies are found in the range from 400 to 599.
*/

const std::string	ErrorReplies::errNoSuchNick(const std::string &nickname, const std::string &wrongNickname)
{
	return (":" + serverName + " 401 " + nickname + " " + wrongNickname + " :No such nick/channel\r\n");
}

const std::string	ErrorReplies::errNoSuchServer(const std::string &nickname, const std::string &serverName)
{
	return (":" + serverName + " 402 " + nickname + " " + serverName + " :No such server\r\n");
} 

const std::string	ErrorReplies::errNoSuchChannel(const std::string &nickname, const std::string &channelName)
{
	return (":" + serverName + " 403 " + nickname + " " + channelName + " :No such channel\r\n");
}

const std::string	ErrorReplies::errCannotSendToChan(const std::string &nickname, const std::string &channelName)
{
	return (":" + serverName + " 404 " + nickname + " " + channelName + " :Cannot send to channel\r\n");
}

const std::string	ErrorReplies::errTooManyTargets(const std::string &nickname, const std::string &target, const std::string &errorCode, const std::string &abortMessage)
{
	return (":" + serverName + " 407 " + nickname + " " + target + " :" + errorCode + " recipients. " + abortMessage + "\r\n");
}

const std::string	ErrorReplies::errNoOrigin(const std::string &nickname)
{
	return (":" + serverName + " 409 " + nickname + " :No origin specified\r\n");
}

const std::string	ErrorReplies::errNoRecipient(const std::string &nickname, const std::string &command)
{
	return (":" + serverName + " 411 " + nickname + " :No recipient given (" + command + ")\r\n");
}

const std::string	ErrorReplies::errNoTextToSend(const std::string &nickname)
{
	return (":" + serverName + " 412 " + nickname + " :No text to send\r\n");
}

const std::string	ErrorReplies::errUnknownCommand(const std::string &nickname, const std::string &command)
{
	return (":" + serverName + " 421 " + nickname + " " + command + " :Unknown command\r\n");
}

const std::string	ErrorReplies::errNoNicknameGiven(std::string nickname)
{
	if (nickname.empty())
		nickname = "unknown";
	return (":" + serverName + " 431 " + nickname + " :No nickname given\r\n");
}

const std::string	ErrorReplies::errErroneusNickname(std::string nickname, const std::string &wrongNickname)
{
	if (nickname.empty())
		nickname = "unknown";
	return (":" + serverName + " 432 " + nickname + " " + wrongNickname + " :Erroneous nickname\r\n");
}

const std::string	ErrorReplies::errNicknameInUse(std::string nickname, const std::string &wrongNickname)
{
	if (nickname.empty())
		nickname = "unknown";
	return (":" + serverName + " 433 " + nickname + " " + wrongNickname + " :Nickname is already in use\r\n");
}

const std::string	ErrorReplies::errUserNotInChannel(const std::string &nickname, const std::string &userNickname, const std::string &channel)
{
	return (":" + serverName + " 441 " + nickname + " " + userNickname + " " + channel + " :They aren't on that channel\r\n");
}

const std::string	ErrorReplies::errNotOnChannel(const std::string &nickname, const std::string &channel)
{
	return (":" + serverName + " 442 " + nickname + " " + channel + " :You're not on that channel\r\n");
}

const std::string	ErrorReplies::errUserOnChannel(const std::string &nickname, const std::string &user, const std::string &channel)
{
	return (":" + serverName + " 443 " + nickname + " " + user + " " + channel + " :is already on channel\r\n");
}

const std::string	ErrorReplies::errNotRegistered(std::string nickname)
{
	return (":" + serverName + " 451 " + nickname + " :You have not registered\r\n");
}

const std::string	ErrorReplies::errNeedMoreParams(const std::string &nickname, const std::string &command)
{
	return (":" + serverName + " 461 " + nickname + " " + command + " :Not enough parameters\r\n");
}

const std::string	ErrorReplies::errAlreadyRegistred(const std::string &nickname)
{
	return (":" + serverName + " 462 " + nickname + " :Unauthorized command (already registered)\r\n");
}

const std::string	ErrorReplies::errPasswdMismatch(std::string nickname)
{
	return (":" + serverName + " 464 " + nickname + " :Password incorrect\r\n");
}

const std::string	ErrorReplies::errKeySet(const std::string &nickname, const std::string &channel)
{
	return (":" + serverName + " 467 " + nickname + " " + channel + " :Channel key already set\r\n");
}

const std::string	ErrorReplies::errChannelIsFull(const std::string &nickname, const std::string &channel)
{
	return (":" + serverName + " 471 " + nickname + " " + channel + " :Cannot join channel (+l)\r\n");
}

const std::string	ErrorReplies::errUnknownMode(const std::string &nickname, const std::string &character, const std::string &channel)
{
	return (":" + serverName + " 472 " + nickname + " " + character + " :is unknown mode char to me for " + channel + "\r\n");
}

const std::string	ErrorReplies::errInviteOnlyChan(const std::string &nickname, const std::string &channel)
{
	return (":" + serverName + " 473 " + nickname + " " + channel + " :Cannot join channel (+i)\r\n");
}

const std::string	ErrorReplies::errBadChannelKey(const std::string &nickname, const std::string &channel)
{
	return (":" + serverName + " 475 " + nickname + " " + channel + " :Cannot join channel (+k)\r\n");
}

const std::string	ErrorReplies::errChanoPrivsNeeded(const std::string &nickname, const std::string &channel)
{
	return (":" + serverName + " 482 " + nickname + " " + channel + " :You're not channel operator\r\n");
}

const std::string	ErrorReplies::errUniqOpPrivsNeeded(const std::string &nickname)
{
	return (":" + serverName + " 485 " + nickname + " :You're not the original channel operator\r\n");
}

// Custom errors
const std::string	ErrorReplies::errTooManyParams(const std::string &nickname, const std::string &command)
{
	return (":" + serverName + " " + nickname + " " + command + " :Too many parameters\r\n");
}

