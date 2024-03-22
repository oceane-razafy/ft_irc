/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandResponses.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:43:49 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/06 15:27:12 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandResponses.hpp"

/*
    RFC 2812
    
    5.1 Command responses

    Numerics in the range from 001 to 099 are used for client-server
    connections only and should never travel between servers.  Replies
    generated in the response to commands are found in the range from 200
    to 399.
*/

const std::string   CommandResponses::rplWelcome(const std::string &nickname, const std::string &username, const std::string &hostname)
{
    return (":" + serverName + " 001 " + nickname + " Welcome to the Internet Relay Network " + nickname + "!" + username + "@" + hostname + "\r\n");
}

const std::string   CommandResponses::rplYourHost(const std::string &nickname)
{
    return (":" + serverName + " 002 " + nickname + " Your host is " + serverName + ", running version " + version + "\r\n");
}

const std::string   CommandResponses::rplCreated(const std::string &nickname)
{
    return (":" + serverName + " 003 " + nickname + " This server was created " + creationDate + "\r\n");
}

const std::string   CommandResponses::rplMyInfo(const std::string &nickname)
{
    return (":" + serverName + " 004 " + nickname + " " + serverName + " " + version + " " + userModes + " " + channelModes + "\r\n");
}

const std::string   CommandResponses::rplUniqOpIs( const std::string &nickname, const std::string &channel, const std::string &newOpNickname)
{
    return (":" + serverName + " 325 " + nickname + " " + channel + " " + newOpNickname) + "\r\n";
}

const std::string   CommandResponses::rplChannelModeIs(const std::string &nickname, const std::string &channel, const std::string &mode, const std::string &modeParams)
{
    return (":" + serverName + " 324 " + nickname + " " + channel + " " + mode + " " + modeParams + "\r\n");
}

const std::string   CommandResponses::rplNoTopic(const std::string &nickname, const std::string &channel)
{
    return (":" + serverName + " 331 " + nickname + " " + channel + " :No topic is set" + "\r\n");
}

const std::string   CommandResponses::rplTopic(const std::string &nickname, const std::string &channel, const std::string &topic)
{
    return (":" + serverName + " 332 " + nickname + " " + channel + " :" + topic + "\r\n");
}

const std::string   CommandResponses::rplInviting( const std::string &nickname, const std::string &channel, const std::string &invitedNickname)
{
    return (":" + serverName + " 341 " + nickname + " " + channel + " " + invitedNickname + "\r\n");
}
