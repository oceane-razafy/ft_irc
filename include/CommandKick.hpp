/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandKick.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/01 18:42:46 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/06 15:00:51 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_KICK_HPP
# define COMMAND_KICK_HPP
# include "ACommand.hpp"
# include "Channel.hpp"

class CommandKick : public ACommand
{

		public:
										CommandKick(std::vector<std::string> params, Client *client, Server *server);
			void						execute(void);
			void						parseKickCmd(void);
			void						tryToKickClient(const std::string &clientToKick, std::string &channelName);
		
		private:
			std::vector<std::string>	_channelNames;
			std::vector<std::string>	_clientsToKick;
			std::string					_comment;
			Channel						*_channel;
			Client						*_clientToKick;
};

#endif