/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configuration.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 20:52:34 by orazafy           #+#    #+#             */
/*   Updated: 2024/02/24 20:36:55 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP
# include <string>
# include <sstream>

class Configuration
{
	public:
		class InvalidPortException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

		class EmptyPasswordException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
		
							Configuration(const std::string &port, const std::string &password);

		int					getPort(void) const;
		const std::string	getPassword(void) const;

	private:
		int			_port;
		std::string	_password;
};

#endif