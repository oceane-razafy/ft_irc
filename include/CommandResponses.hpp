/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandResponses.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 21:58:17 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/08 20:24:44 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*

From RFC 2812

5.1 Command responses

001    RPL_WELCOME
       "Welcome to the Internet Relay Network <nick>!<user>@<host>"

002    RPL_YOURHOST
       "Your host is <servername>, running version <ver>"
       
003    RPL_CREATED
       "This server was created <date>"
       
004    RPL_MYINFO
       "<servername> <version> <available user modes>
       <available channel modes>"
       - The server sends Replies 001 to 004 to a user upon
       successful registration.

325    RPL_UNIQOPIS
       "<channel> <nickname>"
       
324    RPL_CHANNELMODEIS
       "<channel> <mode> <mode params>"
       
331    RPL_NOTOPIC
       "<channel> :No topic is set"
       
332    RPL_TOPIC
       "<channel> :<topic>"
       - When sending a TOPIC message to determine the
       channel topic, one of two replies is sent.  If
       the topic is set, RPL_TOPIC is sent back else
       RPL_NOTOPIC.
       
341    RPL_INVITING
       "<channel> <nick>"
       - Returned by the server to indicate that the
       attempted INVITE message was successful and is
       being passed onto the end client.
*/

#ifndef COMMAND_RESPONSES_HPP
# define COMMAND_RESPONSES_HPP
# include <string>

const std::string    serverName    = "ircserv";
const std::string    version       = "1.0";
const std::string    creationDate  = "02/23/2024";
const std::string    userModes     = "o";
const std::string    channelModes  = "itkol";

class CommandResponses
{
       public:
              static const std::string    rplWelcome(const std::string &nickname, const std::string &username, const std::string &hostname);
              static const std::string    rplYourHost(const std::string &nickname);
              static const std::string    rplCreated(const std::string &nickname);
              static const std::string    rplMyInfo(const std::string &nickname);
              static const std::string    rplUniqOpIs(const std::string &nickname, const std::string &channel, const std::string &newOpCickname);
              static const std::string    rplChannelModeIs(const std::string &nickname, const std::string &channel, const std::string &mode, const std::string &modeParams);
              static const std::string    rplNoTopic(const std::string &nickname, const std::string &channel);
              static const std::string    rplTopic(const std::string &nickname, const std::string &channel, const std::string &topic);
              static const std::string    rplInviting( const std::string &nickname, const std::string &channel, const std::string &invitedNickname);
};

#endif
