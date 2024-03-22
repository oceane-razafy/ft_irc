/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeInfo.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 18:49:15 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/06 18:49:54 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODE_INFO_HPP
#define MODE_INFO_HPP

#include <string>

// For MODE and its loggers
struct ModeInfo
{
    char modeOption;     // '+' ou '-'
    char mode;           // 'i', 't', 'o', 'k', 'l'
    std::string optionalArg; // Optional arg if necessary
};

#endif
