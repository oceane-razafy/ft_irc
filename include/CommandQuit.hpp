/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandQuit.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 18:41:53 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/06 15:14:07 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_QUIT_HPP
# define COMMAND_QUIT_HPP
# include "ACommand.hpp"

class CommandQuit : public ACommand
{

		public:
						CommandQuit(std::vector<std::string> params, Client *client, Server *server);
			void		execute(void);
		
		private:
			Channel		*_channel;
			std::string	_comment;
};

#endif