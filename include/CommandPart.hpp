/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandPart.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 23:07:31 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/02 23:20:49 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_PART_HPP
# define COMMAND_PART_HPP
# include "ACommand.hpp"

class CommandPart : public ACommand
{

		public:
										CommandPart(std::vector<std::string> params, Client *client, Server *server);
			void						execute(void);
		
		private:
			std::vector<std::string>	_channelNames;
			Channel						*_channel;
			std::string					_comment;
};

#endif