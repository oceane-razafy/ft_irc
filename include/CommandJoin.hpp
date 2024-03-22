/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandJoin.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 22:39:58 by valentin          #+#    #+#             */
/*   Updated: 2024/03/06 15:00:27 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_JOIN_HPP
# define COMMAND_JOIN_HPP
# include "ACommand.hpp"

struct JoinInfo {
	std::string	channelName;
	std::string	triedKey;
};

class CommandJoin : public ACommand
{
	public:
									CommandJoin(std::vector<std::string> params, Client *client, Server *server);
		void						execute(void);

		class NotOperatorException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

		class NoSuchChannelException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

	private:
		bool						_shouldLeaveAllChannels;
		std::vector<JoinInfo>		_joinInfos;

		std::vector<JoinInfo>		_createJoinInfo(const std::vector<std::string>& channelsNames, const std::vector<std::string>& triedKeys);
		void						_leaveAllChannels(void);
		void						_joinChannelProcess(Channel *currentChannel, std::string channelName);
		void						_createChannelProcess(std::string channelName);
};

#endif
