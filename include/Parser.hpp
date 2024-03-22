/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 20:52:37 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/19 13:24:44 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP
# include "Server.hpp"
# include "Client.hpp"
# include "Utils.hpp"
# include <vector>

class Client;

class Server;

class Parser
{
	public:
									Parser(Server *server);
		std::vector<std::string>	getMessages(void) const;
		void						parseRequest(Client *client);

		class EmptyMessageException: public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

		class InvalidPrefixException: public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

	private:
		Server 						*_server;
		std::string					_serverReply;
		std::vector<std::string>	_messages;

		void 						_splitMessages(const std::string & request, Client *client);
		std::vector<std::string>	_tokenizeMessage(std::string message, Client *client);
		std::vector<std::string>	_getParamsFromTokens(std::vector<std::string> tokens) const;
		void						_processMessage(Client *client, std::string message);
};

#endif