/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandPing.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:57:00 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/06 15:12:19 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_PONG_HPP
# define COMMAND_PONG_HPP
# include "ACommand.hpp"

class CommandPing : public ACommand
{

		public:
					CommandPing(std::vector<std::string> params, Client *client, Server *server);
			void	execute(void);

		class NoSuchServerException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};
};

#endif
