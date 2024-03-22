/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandNick.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 16:55:54 by valentin          #+#    #+#             */
/*   Updated: 2024/02/28 01:49:17 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_NICK_HPP
# define COMMAND_NICK_HPP
# include "ACommand.hpp"

/*
	3.1.2 Nick message

	Command: NICK
	Parameters: <nickname>

	NICK command is used to give user a nickname or change the existing one.

	Numeric Replies:

	ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
	ERR_NICKNAMEINUSE               ERR_NICKCOLLISION
	ERR_UNAVAILRESOURCE             ERR_RESTRICTED

	Examples:

	NICK Wiz				; Introducing new nick "Wiz" if session is
							still unregistered, or user changing his
							nickname to "Wiz"

	NICK Kilroy				; User changing nickname from Wiz to Kilroy

	:WiZ!jto@tolsun.oulu.fi NICK Kilroy

							; Server telling that WiZ changed his
							nickname to Kilroy.
*/
class CommandNick : public ACommand
{
	public:
					CommandNick(std::vector<std::string> params, Client *client, Server *server);
		void		execute(void);

		class NoPasswordProvidedException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class NoNicknameGivenException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class ErroneousNicknameException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class NicknameInUseException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

	private:
		bool		_isNicknameValid(std::string nickname) const;
		bool		_isNicknameAlreadyInUse(std::string nickname) const;

		std::string	_newNickname;
};

#endif