/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: valentin <valentin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/25 14:25:58 by valentin          #+#    #+#             */
/*   Updated: 2024/03/12 22:06:23 by valentin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
#include "Logger.hpp" // to debug

bool	Utils::isAllCharactersDigit(const std::string input)
{
	for (std::string::const_iterator it = input.begin(); it != input.end(); ++it)
	{
		if (!(isdigit(*it)))
			return (false);
	}
	return (true);
}

std::string Utils::toLower(const std::string input)
{
	std::string result;
	for (std::string::const_iterator it = input.begin(); it != input.end(); ++it)
		result += std::tolower(static_cast<unsigned char>(*it));
	return result;
}

std::string Utils::toLowerChannelName(std::string channelName)
{
	// Logger::logValue(LIGHT_CYAN, "[UTILS] channelName", formatMessage(channelName));
	std::string tmp(channelName.size(), '\0');
	if (channelName[0] == '!')
	{
		size_t i = 0;
        while (i < 6 && i < channelName.size())
		{
			tmp[i] = channelName[i];
			i++;
		}
		if (i < channelName.size())
		{
			std::string remainder = Utils::toLower(channelName.substr(i));
			int j = 0;

			while (i < channelName.size())
				tmp[i++] = remainder[j++];
		}
		channelName = tmp;
	}
	else
		channelName = Utils::toLower(channelName);
	// Logger::logValue(LIGHT_CYAN, "[UTILS] final channel name", formatMessage(channelName));
	return (channelName);
}



std::string Utils::toUpper(const std::string & input)
{
	std::string result;
	for (std::string::const_iterator it = input.begin(); it != input.end(); ++it)
		result += std::toupper(static_cast<unsigned char>(*it));
	return result;
}

bool Utils::isSpecialChar(char c)
{
	std::string specialCharsSet = "[]\\`_^{|}";
	for (size_t i = 0; i < specialCharsSet.size(); ++i)
	{
		if (c == specialCharsSet[i])
			return true;
	}
	return false;
}

/*
	RFC 2812:
	
  	key        =  1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )
                  ; any 7-bit US_ASCII character,
                  ; except NUL, CR, LF, FF, h/v TABs, and " "
*/

void	Utils::checkPasswordKeyValidity(std::string password)
{
	// CR, LF, FF, h/v TABs, and " "
    std::string forbiddenCharacters = "\r\n\f\t\v ";

	// Password/Key cannot be longer than 23 characters
	if ((password.size() > 23)
	// It SHALL NOT contain CR, LF, FF, h/v TABs, and " "
	|| (password.find_first_of(forbiddenCharacters) != std::string::npos))
		throw InvalidPasswordKeyException();
	
	// Password/Key has to contain only any 7-bit US_ASCII character
	for (size_t i = 0; i < password.size(); i++)
	{
		    if (!(static_cast<unsigned char>(password[i]) > 0
				&& static_cast<unsigned char>(password[i]) <= 127))
				throw InvalidPasswordKeyException();	
	}
}

std::string Utils::formatMessage(std::string message)
{
	std::string formattedMessage;
	for(size_t i = 0; i < message.length(); i++)
	{
		switch (message[i])
		{
			case '\f':
				formattedMessage += "\\f";
				break ;
			case '\n':
				formattedMessage += "\\n";
				break ;
			case '\r':
				formattedMessage += "\\r";
				break ;
			case '\b':
				formattedMessage += "\\b";
				break ;
			case '\t':
				formattedMessage += "\\t";
				break ;
			case '\a':
				formattedMessage += "\\a";
				break ;
			case '\v':
				formattedMessage += "\\v";
				break ;
			case '\0':
				formattedMessage += "\\0";
				break ;
			default:
				formattedMessage += message[i];
		}
	}
	return formattedMessage;
}

const char* Utils::InvalidPasswordKeyException::what() const throw()
{
	return ("ERROR: Invalid password/key format.");
}
