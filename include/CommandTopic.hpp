/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandTopic.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 03:41:06 by orazafy           #+#    #+#             */
/*   Updated: 2024/02/29 04:20:55 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_TOPIC_HPP
# define COMMAND_TOPIC_HPP
# include "ACommand.hpp"

class CommandTopic : public ACommand
{

		public:
								CommandTopic(std::vector<std::string> params, Client *client, Server *server);
			void				execute(void);
		
		private:
			Channel				*_channel;
			std::string			_topic;
};

#endif