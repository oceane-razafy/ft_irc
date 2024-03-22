/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandInvite.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 16:57:00 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/01 00:47:20 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_INVITE_HPP
# define COMMAND_INVITE_HPP
# include "ACommand.hpp"

class CommandInvite : public ACommand
{

		public:
					CommandInvite(std::vector<std::string> params, Client *client, Server *server);
			void	execute(void);

			class UserOnChannelException: public std::exception
			{
				public:
						virtual const char* what() const throw();
			};
		
		private:
			Channel	*_channel;
};

#endif