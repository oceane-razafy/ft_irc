/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: valentin <valentin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/25 14:24:03 by valentin          #+#    #+#             */
/*   Updated: 2024/03/12 21:56:25 by valentin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <string>
# include <cctype>

class Utils
{
	public:
		static bool			isAllCharactersDigit(const std::string input);
		static std::string 	toLower(const std::string input);
		static std::string	toLowerChannelName(std::string channelName);
		static std::string 	toUpper(const std::string &input);
		static bool			isSpecialChar(char c);
		static void		 	checkPasswordKeyValidity(std::string password);
		static std::string 	formatMessage(std::string message);

		class InvalidPasswordKeyException: public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
};

#endif