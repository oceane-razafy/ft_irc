/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandPrivmsg.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: valentin <valentin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 20:06:04 by valentin          #+#    #+#             */
/*   Updated: 2024/03/12 21:54:53 by valentin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_PRIVMSG_HPP
# define COMMAND_PRIVMSG_HPP

# include "ACommand.hpp"
# include <sstream>

struct MsgToInfo {
	std::string nickname;
	std::string channelName;
	std::string type; // Which type of msgto ?
};

class CommandPrivmsg: public ACommand
{
	public:
						CommandPrivmsg(std::vector<std::string> params, Client *client, Server *server);
		void			execute(void);

		class NoSuchNickException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class NoRecipientException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class NoTextToSendException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class CannotSendToChanException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class TooManyTargetsException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		/* ****************************************************************** */

		class HostnameDoesNotMatchException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class ServernameDoesNotMatchException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class NoNicknameAssociatedException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		/* ****************************************************************** */

	private:
		MsgToInfo		_msgToInfo;
		std::string		_msgToSend;

		MsgToInfo		_parseMsgTo(const std::string& msgto);
		std::string		_formatMsgToSend(const std::string& rawMsgToSend);
		MsgToInfo		_checkUserHostServerValidity(const std::string& msgto, size_t percentPos, size_t atPos);
		MsgToInfo		_checkUserHostValidity(const std::string& msgto, size_t percentPos);
		MsgToInfo		_checkNickUserHostValidity(const std::string& msgto, size_t exclamPos, size_t atPos);
		MsgToInfo		_checkUserServerValidity(const std::string& msgto, size_t atPos);


};

#endif