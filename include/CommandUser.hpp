/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandUser.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: valentin <valentin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 18:50:51 by valentin          #+#    #+#             */
/*   Updated: 2024/03/12 21:40:32 by valentin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_USER_HPP
# define COMMAND_USER_HPP
# include "ACommand.hpp"

class CommandUser : public ACommand
{
	public:
					CommandUser(std::vector<std::string> params, Client *client, Server *server);
		void		execute(void);

		class NoPwdNoNickException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class NoNickException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class AlreadyRegisteredException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class InvalidUsernameException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

	private:
		std::string	_username;
		std::string	_hostname;
		std::string	_realname;

		std::string	_getClientIpAdrr(int clientFd);
		bool		_isValidUsername(const std::string& username);

};

#endif