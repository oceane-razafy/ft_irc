/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ACommand.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: valentin <valentin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 01:29:49 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/12 21:54:48 by valentin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACOMMAND_HPP
# define ACOMMAND_HPP
# include "Server.hpp"
# include "ErrorReplies.hpp"
# include "CommandResponses.hpp"
# include "Colors.hpp"
# include <vector>
# include <string>
# include <iostream>
# include <cctype>

class ACommand
{
	public:
									ACommand(std::vector<std::string> params, Client *client, Server *server);
	
		// Each derived class needs to implement this one
		virtual void				execute(void) = 0;

		// Helpers to execute each concrete command class
		std::vector<std::string> 	splitParametersList(const std::string& parametersList);
		void						checkParamsNb(const std::string &command, long unsigned int min, long unsigned int max);
		void						checkIsRegistered(void);
		Channel						*getChannel(std::string channelName);
		void						isNicknameInServer(const std::string &nickname);
		void						isNicknameInChannel(const std::string &nickname, std::string channelName, bool forClientWhoAsked);

		// Almost all derived classes will need these exceptions
		class NotRegisteredException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};
		
		class TooManyParamsException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};
		
		class NeedMoreParamsException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class	NoSuchNickException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class	NoSuchChannelException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class	ClientNotInChannelException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class	TargetNotInChannelException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class	NotOperatorException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

	protected:
		// Every derived class has those attributes
		std::vector<std::string>	_params;
		Client 						*_client;
		Server 						*_server;
		std::string					_serverReply;
};

#endif