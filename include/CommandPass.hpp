/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandPass.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/25 18:48:59 by valentin          #+#    #+#             */
/*   Updated: 2024/02/28 01:51:08 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_PASS_HPP
# define COMMAND_PASS_HPP
# include "ACommand.hpp"

/*
	3.1.1 Password message

	Command: PASS
	Parameters: <password>

	The PASS command is used to set a 'connection password'. The
	optional password can and MUST be set before any attempt to register
	the connection is made.  Currently this requires that user send a
	PASS command before sending the NICK/USER combination.

	Numeric Replies:
		461 ERR_NEEDMOREPARAMS              462 ERR_ALREADYREGISTRED

	Example:
		PASS secretpasswordhere
*/

/*
	464 ERR_PASSWDMISMATCH
	
	":Password incorrect"

	- Returned to indicate a failed attempt at registering
	a connection for which a password was required and
	was either not given or incorrect.
*/

class CommandPass : public ACommand
{
	public:
					CommandPass(std::vector<std::string> params, Client *client, Server *server);
		void		execute(void);

		class AlreadyRegisteredException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class PassAlreadyProvidedException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

	private:
		std::string	_triedPassword;
};

#endif