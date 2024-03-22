/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorReplies.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 21:59:25 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/20 14:48:11 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*

From RFC 2812

5.2 Error Replies

401  ERR_NOSUCHNICK
     "<nickname> :No such nick/channel"
     - Used to indicate the nickname parameter supplied to a
     command is currently unused.
     
402  ERR_NOSUCHSERVER
     "<server name> :No such server"
     - Used to indicate the server name given currently
     does not exist.
     
403  ERR_NOSUCHCHANNEL
     "<channel name> :No such channel"
     - Used to indicate the given channel name is invalid.

404  ERR_CANNOTSENDTOCHAN
     "<channel name> :Cannot send to channel"
     - Sent to a user who is either (a) not on a channel
     which is mode +n or (b) not a chanop (or mode +v) on
     a channel which has mode +m set or where the user is
     banned and is trying to send a PRIVMSG message to
     that channel.

407  ERR_TOOMANYTARGETS
     "<target> :<error code> recipients. <abort message>"

     - Returned to a client which is attempting to send a
     PRIVMSG/NOTICE using the user@host destination format
     and for a user@host which has several occurrences.

     - Returned to a client which trying to send a
     PRIVMSG/NOTICE to too many recipients.

     - Returned to a client which is attempting to JOIN a safe
     channel using the shortname when there are more than one
     such channel.

409  ERR_NOORIGIN
     ":No origin specified"
     - PING or PONG message missing the originator parameter.

411  ERR_NORECIPIENT
     ":No recipient given (<command>)"
     
412  ERR_NOTEXTTOSEND
     ":No text to send"
     
     - 412 - 415 are returned by PRIVMSG to indicate that
     the message wasn't delivered for some reason.
     
421  ERR_UNKNOWNCOMMAND
     "<command> :Unknown command"
     - Returned to a registered client to indicate that the
     command sent is unknown by the server.

431  ERR_NONICKNAMEGIVEN
     ":No nickname given"
     - Returned when a nickname parameter expected for a
     command and isn't found.

432  ERR_ERRONEUSNICKNAME
     "<nick> :Erroneous nickname"
     - Returned after receiving a NICK message which contains
     characters which do not fall in the defined set.  See
     section 2.3.1 for details on valid nicknames.

433  ERR_NICKNAMEINUSE
     "<nick> :Nickname is already in use"
     - Returned when a NICK message is processed that results
     in an attempt to change to a currently existing
     nickname.
     
441  ERR_USERNOTINCHANNEL
     "<nick> <channel> :They aren't on that channel"
     - Returned by the server to indicate that the target
     user of the command is not on the given channel.

442  ERR_NOTONCHANNEL
     "<channel> :You're not on that channel"
     - Returned by the server whenever a client tries to
     perform a channel affecting command for which the
     client isn't a member.

443  ERR_USERONCHANNEL
     "<user> <channel> :is already on channel"
     - Returned when a client tries to invite a user to a
     channel they are already on.

451  ERR_NOTREGISTERED
     ":You have not registered"
     - Returned by the server to indicate that the client
     MUST be registered before the server will allow it
     to be parsed in detail.

461  ERR_NEEDMOREPARAMS
     "<command> :Not enough parameters"
     - Returned by the server by numerous commands to
     indicate to the client that it didn't supply enough
     parameters.

462  ERR_ALREADYREGISTRED
     ":Unauthorized command (already registered)"
     - Returned by the server to any link which tries to
     change part of the registered details (such as
     password or user details from second USER message).

464  ERR_PASSWDMISMATCH
     ":Password incorrect"
     - Returned to indicate a failed attempt at registering
     a connection for which a password was required and
     was either not given or incorrect.

467  ERR_KEYSET
     "<channel> :Channel key already set"

471  ERR_CHANNELISFULL
     "<channel> :Cannot join channel (+l)"

472  ERR_UNKNOWNMODE
     "<char> :is unknown mode char to me for <channel>"

473  ERR_INVITEONLYCHAN
     "<channel> :Cannot join channel (+i)"

475  ERR_BADCHANNELKEY
     "<channel> :Cannot join channel (+k)"

482  ERR_CHANOPRIVSNEEDED
     "<channel> :You're not channel operator"
     - Any command requiring 'chanop' privileges (such as
     MODE messages) MUST return this error if the client
     making the attempt is not a chanop on the specified
     channel.
     
485  ERR_UNIQOPPRIVSNEEDED
     ":You're not the original channel operator"
     - Any MODE requiring "channel creator" privileges MUST
     return this error if the client making the attempt is not
     a chanop on the specified channel.
*/


#ifndef ERROR_REPLIES_HPP
# define ERROR_REPLIES_HPP
# include "CommandResponses.hpp"
# include <string>
class ErrorReplies
{
     public:
          static const std::string errNoSuchNick(const std::string &nickname, const std::string &wrongNickname);
          static const std::string errNoSuchServer(const std::string &nickname, const std::string &serverName); 
          static const std::string errNoSuchChannel(const std::string &nickname, const std::string &channelName);
          static const std::string errCannotSendToChan(const std::string &nickname, const std::string &channelName);
          static const std::string errTooManyTargets(const std::string &nickname, const std::string &target, const std::string &errorCode, const std::string &abortMessage);
          static const std::string errNoOrigin(const std::string &nickname);
          static const std::string errNoRecipient(const std::string &nickname, const std::string &command);
          static const std::string errNoTextToSend(const std::string &nickname);
          static const std::string errUnknownCommand(const std::string &nickname, const std::string &command);
          static const std::string errNoNicknameGiven(std::string nickname);
          static const std::string errErroneusNickname(std::string nickname, const std::string &wrongNickname); // there is the typo 'Erroneus' on the actual protocol (rfc 2812)
          static const std::string errNicknameInUse(std::string nickname, const std::string &wrongCickname);
          static const std::string errUserNotInChannel(const std::string &nickname, const std::string &userNickname, const std::string &channel);
          static const std::string errNotOnChannel(const std::string &nickname, const std::string &channel);
          static const std::string errUserOnChannel(const std::string &nickname, const std::string &user, const std::string &channel);
          static const std::string errNotRegistered(std::string nickname);
          static const std::string errNeedMoreParams(const std::string &nickname, const std::string &command);
          static const std::string errAlreadyRegistred(const std::string &nickname); // there is the typo 'Registred' on the actual protocol (rfc 2812)
          static const std::string errPasswdMismatch(std::string nickname);
          static const std::string errKeySet(const std::string &nickname, const std::string &channel);
          static const std::string errChannelIsFull(const std::string &nickname, const std::string &channel);
          static const std::string errUnknownMode(const std::string &nickname, const std::string &character, const std::string &channel);
          static const std::string errInviteOnlyChan(const std::string &nickname, const std::string &channel);
          static const std::string errBadChannelKey(const std::string &nickname, const std::string &channel);
          static const std::string errChanoPrivsNeeded(const std::string &nickname, const std::string &channel);
          static const std::string errUniqOpPrivsNeeded(const std::string &nickname);

          // CUSTOM errors
          static const std::string	errTooManyParams(const std::string &nickname, const std::string &command);

};

#endif
