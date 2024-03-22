/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 21:08:05 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/12 17:56:15 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "Utils.hpp"
# include <poll.h>
# include <string>
# include <vector>
# include <queue>
# include <algorithm>

class Client
{
	public:
									Client(pollfd pollfd);

		// Getters
		pollfd						getPollfd(void) const;
		int							getFd(void) const;
		const std::string			getRequest(void);
		std::string					getNickname(void) const;
		std::string					getUsername(void) const;
		std::string					getHostname(void) const;
		std::string					getRealname(void) const;
		std::string					getHostmask(void);
		bool						getIsRegistered(void) const;
		bool						getPASSRequestDone(void) const;
		bool						getNICKRequestDone(void) const;
		bool						getHasQuit(void) const;
		std::vector<std::string>	getChannels(void) const;
		std::queue<std::string>		&getPendingServerMsgs(void);
		
		// Setters
		void						setNickname(const std::string &nickname);
		void						setUsername(const std::string &username);
		void						setHostname(const std::string &hostname);
		void						setRealname(const std::string &realname);
		void						setIsRegistered(bool isRegistered);
		void						setPASSRequestDone(bool PASSRequestDone);
		void						setNICKRequestDone(bool NICKRequestDone);
		void						setHasQuit(bool hasQuit);

		// Store the client request
		void						storeRequestData(const std::string &data);
		void						updateRequest(const std::string &data);

		// Functions to handle client channels
		void						addChannel(std::string newChannel);
		void						removeChannel(std::string channelName);
		bool						isPartOfThisChannel(std::string channelName);

		// Add a new pending server message
		void						addNewServerMsg(const std::string &msg);
		
	private:
		pollfd						_pollfd;
		int							_fd;
		std::string					_request;

		std::string					_nickname;
		std::string					_username;
		std::string					_hostname;
		std::string					_realname;

		bool						_isRegistered;
		bool						_PASSRequestDone;
		bool						_NICKRequestDone;

		bool						_hasQuit;
		
		std::vector<std::string>	_channels;
		std::queue<std::string>		_pendingServerMsgs;
};

#endif