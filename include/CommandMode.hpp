/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandMode.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 16:52:16 by valentin          #+#    #+#             */
/*   Updated: 2024/03/20 18:28:36 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_MODE_HPP
# define COMMAND_MODE_HPP
# include "ACommand.hpp"
# include "ModeInfo.hpp"

class CommandMode : public ACommand
{
	public:
									CommandMode(std::vector<std::string> params, Client *client, Server *server);
		void						execute(void);
		
		void						replyUnknownModeError(char mode);
		void						replyNotNumericOrTooLargeNumber(void);
		void						trackExecutedModes(char modeOption, char mode);

		class InvalidKeyProvidedException: public std::exception
		{
			public:
					virtual const char* what() const throw();
		};

	private:
		Channel						*_channel;
		
		// For parsing
		std::vector<ModeInfo>		_modes;
		std::vector<std::string>	_cleanedParams;
		void						_parseModes(void);
		void						_cleanModeCmd(void);	
		
		// For RPL_CHANNELMODEIS
		std::string					_modesStr;
		std::string					_modeParamsStr;	
};

#endif