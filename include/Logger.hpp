/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 17:45:52 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/20 15:21:51 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
# define LOGGER_HPP
# include "Colors.hpp"
# include "ModeInfo.hpp"
# include <string>
# include <iostream>
# include <poll.h>
# include <vector>
# include <queue>
# include <map>


class Logger
{
	public:

	// To log a vector
		template <typename T>
		static void	logVector(const std::string &color, const std::string &title, std::vector<T> vector)
		{
			std::cout << color << title << std::endl;
			for (size_t i = 0; i < vector.size(); i++)
				std::cout << "\t - [" << i << "] = " << vector[i] << std::endl;
			std::cout << RESET;
		}

		template <typename T>
		static void	logQueue(const std::string &color, const std::string &title, std::queue<T> queue)
		{
			std::cout << color << title << std::endl;
			size_t size = queue.size();
			for (size_t i = 0; i < size; i++)
			{
				std::cout << "\t - [" << i << "] = " << queue.front() << std::endl;
				queue.pop();
			}
			std::cout << RESET;
		}

	// To log a map
		template<typename Key, typename Value>
		static void	logMapKey(const std::string &color, const std::string &title, std::map<Key,Value> map)
		{
			std::cout << color << title << std::endl;
			typename std::map<Key,Value>::iterator it;
			for (it = map.begin(); it != map.end(); it++)
				std::cout << "\t - " << it->first << std::endl;
			std::cout << RESET;
		}
	
	// To log  a value
		template<typename T>
		static void	logValue(const std::string &color, const std::string &msg, T value)
		{
			std::cout << color << msg << ": " << value << RESET << std::endl;
		}
		
	// Common logs
		static void log(const std::string &color, const std::string &msg);
		static void logDebug(const std::string &color, const std::string &msg);
		static void logInfo(const std::string &color, const std::string &msg);

	// Server
		static void	logPollSet(std::vector<pollfd> pollfds);
		static void	logBeforeParsing(const std::string &color, const std::string &buffer, const std::string &request);
		static void	logAfterParsing(const std::string &color, const std::string &buffer, const std::string &request);
		
	// Parser
		static void	logMessages(const std::string &color, std::vector<std::string> messages);
		static void	logTokens(const std::string &color, std::vector<std::string> tokens, const std::string &message);

	// Mode
		static void	logParams(const std::string &color, std::vector<std::string> params);
		static void	logModes(const std::string &color, std::vector<ModeInfo> modes);
};

#endif