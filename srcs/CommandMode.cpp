/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandMode.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 14:31:07 by valentin          #+#    #+#             */
/*   Updated: 2024/03/21 13:41:10 by orazafy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandMode.hpp"

/*
	RFC 2812 
	
	Command: MODE
   	Parameters: <channel> *( ( "-" / "+" ) *<modes> *<modeparams> )

	The MODE command is provided so that users may query and change the
	characteristics of a channel
	
	Numeric Replies:

           ERR_NEEDMOREPARAMS              	ERR_KEYSET
           ERR_NOCHANMODES (not in subject) ERR_CHANOPRIVSNEEDED
           ERR_USERNOTINCHANNEL            	ERR_UNKNOWNMODE
		   
           RPL_CHANNELMODEIS
           RPL_BANLIST (not in subject)     RPL_ENDOFBANLIST (not in subject)
           RPL_EXCEPTLIST (not in subject)  RPL_ENDOFEXCEPTLIST (not in subject)
           RPL_INVITELIST                  	RPL_ENDOFINVITELIST (not in subject)
           RPL_UNIQOPIS (not in subject)

	CUSTOM
			ERR_NOSUCHCHANNEL (added by us)
*/

/* ==================================================================================
    * Constructor
================================================================================== */

CommandMode::CommandMode(std::vector<std::string> params, Client *client, Server *server)
	: ACommand(params, client, server)
{
	Logger::logVector(BRIGHT_CYAN, "[MODE] _params", _params);
	
	// Check if client who issued the command is registered
	checkIsRegistered();
	
	// Check the number of parameters provided (ERR_NEEDMOREPARAMS)
	/*
		RFC 2812:
		"Each IRC message may consist of up to three main parts: the prefix
		(OPTIONAL), the command, and the command parameters (maximum of
		fifteen (15)).
	*/
	checkParamsNb("MODE", 1, 15);

	/*  
		RFC 2812 does not mention anything if channel does not exist
		neither if server should not care, like INVITE
		or if server needs to reply an error ERR_NOSUCHCHANNEL
		
		But we still try to get the channel
		and throw an error if necessary (ERR_NOSUCHCHANNEL)
	*/
	_channel = getChannel(_params[0]);

	Logger::logValue(BRIGHT_CYAN, "[MODE] channel name", _channel->getName());
	Logger::logValue(BRIGHT_CYAN, "[MODE] channel info", _channel->getListOfAllUsersNamesInChannelAsStr());

	// Check if the client who issued the command has the operator privilege to do it
	if (_channel->isOperator(client->getNickname()) == false)
	{
		_serverReply = ErrorReplies::errChanoPrivsNeeded(_client->getNickname(), _params[0]);
		_server->handleNewMsgToClient(_serverReply, _client);
		throw NotOperatorException();
	}

	// Parse the MODE command line only if there is more than one parameter
	if (_params.size() > 1)
	{
		_cleanModeCmd();
		_parseModes();
	}
}

/* ==================================================================================
    * Parsing the parameters
================================================================================== */

/*
	EXAMPLE: "MODE #chan +ik secret +l 50"

	input =  [ "#chan", "+ik", "secret", "+l", "50" ]
	output = [ "#chan", "+ik+l", "secret", "50" ]
*/
void CommandMode::_cleanModeCmd(void)
{
	std::vector<std::string> modeArgsAccumulator;

	_cleanedParams.push_back(_params[0]);

	std::string modeSequenceAccumulator = _params[1];

	for (size_t i = 2; i < _params.size(); ++i)
	{
		const std::string& param = _params[i];
		if (!param.empty() && (param[0] == '+' || param[0] == '-'))
			modeSequenceAccumulator += param;
		else
			modeArgsAccumulator.push_back(param);
	}

	_cleanedParams.push_back(modeSequenceAccumulator);
	std::vector<std::string>::iterator it = modeArgsAccumulator.begin();
	for ( ; it != modeArgsAccumulator.end(); ++it)
		_cleanedParams.push_back(*it);

	_params = _cleanedParams;

	Logger::logParams(BRIGHT_CYAN, _params);
}

/*
 Example 1: "MODE #chan -i+kl password 20"

	params = [ "#chan", "-i+kl", "password", "20" ]
	optionalArgIndex :               2         3

	For this example, we then have:
	_modes = [ { '-', 'i', "" }, { '+', 'k', "password" }, { '+', 'l', "20" } ]

 Example 2: "MODE #chan -ik+l 20"

	params = [ "#chan", "-ik+l", "20" ]
	optionalArgIndex :            2

	For this example, we then have:
	_modes = [ { '-', 'i', "" }, { '-', 'k', "" }, { '+', 'l', "20" } ]
*/
void CommandMode::_parseModes(void)
{
	const std::string& modeSequence = _params[1];
	std::vector<ModeInfo> modes;
	char currentModeOption = 0;
	size_t optionalArgIndex = 2;
	std::string tempKey = "";

	for (size_t i = 0; i < modeSequence.length(); ++i)
	{
		char modeChar = modeSequence[i];

		if (modeChar == '+' || modeChar == '-')
		{
			currentModeOption = modeChar;
		}
		else
		{
			ModeInfo modeInfo;
			modeInfo.modeOption = currentModeOption;
			modeInfo.mode = modeChar;
			modeInfo.optionalArg = ""; // By default, no optional arg

			// If mode requires an arg and such an arg is provided
			if (modeChar == 'k' && currentModeOption == '-')
			{
				if ((_channel->isKeySet() || !tempKey.empty()) && optionalArgIndex < _params.size())
					modeInfo.optionalArg = _params[optionalArgIndex++];
				else if ((_channel->isKeySet() || !tempKey.empty()) && optionalArgIndex == _params.size())
				{
					_serverReply = ErrorReplies::errNeedMoreParams(_client->getNickname(), "MODE");
					_server->handleNewMsgToClient(_serverReply, _client);
					throw NeedMoreParamsException();
				}
			}
			else if (modeChar == 'k' && currentModeOption == '+' && optionalArgIndex < _params.size())
			{
				modeInfo.optionalArg = _params[optionalArgIndex++];
				if (!modeInfo.optionalArg.empty())
				{
					try
					{
						Utils::checkPasswordKeyValidity(modeInfo.optionalArg);
						tempKey = modeInfo.optionalArg;
					}
					catch (const std::exception & e) {}
				}
			}
			else if (((modeChar == 'l' && currentModeOption == '+')
					|| modeChar == 'o')
					&& optionalArgIndex < _params.size())
			{
				modeInfo.optionalArg = _params[optionalArgIndex++];
			}

			modes.push_back(modeInfo);
		}
	}
	if (optionalArgIndex != _params.size())
	{
		_serverReply = ErrorReplies::errTooManyParams(_client->getNickname(), "MODE");
		_server->handleNewMsgToClient(_serverReply, _client);
		throw TooManyParamsException();
	}
	_modes = modes;

	Logger::logModes(BRIGHT_CYAN, _modes);
}

/* ==================================================================================
    * Execution
================================================================================== */

/*
	_modes = [
	{ '+', 'i', "" },		executed
	{ '+', 'k', "a-aa" },	executed
	{ '+', 'l', "10" },		executed
	{ '+', '5', "" },		UNKNOWN MODE exception
	{ '+', '0', "" },		UNKNOWN MODE exception
	{ '-', 'i', "" }		executed
*/

/*
	struct ModeInfo {
		char		modeOption; // '+' ou '-'
		char 		mode; // 'i', 't', 'o', 'k', 'l'
		std::string	optionalArg; // Optional arg if necessary
	};
*/
void		CommandMode::execute(void)
{
	std::vector<ModeInfo>::iterator it;

	// MODE #channel also works -> server is going to reply RPL_CHANNELMODEIS
	if (_modes.empty())
	{
		_server->handleNewMsgToClient(_channel->buildReplyChannelModeIs(_client->getNickname()), _client);
		return ;
	}

	// Check if for modes with parameter, the parameter does exist
	for (it = _modes.begin(); it != _modes.end(); it++)
	{
		if ((it->mode == 'l' && it->modeOption == '+')
		|| (it->mode == 'k' && it->modeOption == '+')
		|| (it->mode == 'o' && it->modeOption == '+')
		|| (it->mode == 'o' && it->modeOption == '-'))
		{
			if ((it->optionalArg).empty())
			{
				_serverReply = ErrorReplies::errNeedMoreParams(_client->getNickname(), "MODE");
				_server->handleNewMsgToClient(_serverReply, _client);
				throw NeedMoreParamsException();
			}
		}
	}
	
	/*
		RFC 2812:
		3.2.3 Channel mode message
		"Note that there is a maximum limit of three (3) changes per
   		command for modes that take a parameter"
	*/
	int	modeWithParamsDone = 0;
	bool warningSent = false;
	
	for (it = _modes.begin(); it != _modes.end(); it++)
	{
		if (modeWithParamsDone == 3 && warningSent == false)
		{
			_serverReply = ":" + serverName + " Maximum limit of 3 changes per command for modes that take a parameter has been reached\r\n";
			_server->handleNewMsgToClient(_serverReply, _client);
			// break ;
			warningSent = true;
		}
		if (it->mode == 'i')
		{
			if (it->modeOption == '+')
				_channel->setIsInviteOnly(_client->getNickname(), true);
			else
				_channel->setIsInviteOnly(_client->getNickname(), false);
			trackExecutedModes(it->modeOption, it->mode);
		}
		else if (it->mode == 't')
		{
			if (it->modeOption == '+')
				_channel->setIsTOPICforOperators(_client->getNickname(), true);
			else
				_channel->setIsTOPICforOperators(_client->getNickname(), false);
			trackExecutedModes(it->modeOption, it->mode);
		}
		else if (it->mode == 'k')
		{
			if ((!(it->optionalArg.empty())) && modeWithParamsDone == 3)
				continue ;
			try
			{
				if (it->modeOption == '+')
				{
					_channel->setKey(_client->getNickname(), it->optionalArg);
				}
				else
				{
					if (_channel->isChannelKey(it->optionalArg) == false)
						throw InvalidKeyProvidedException();
					_channel->setKey(_client->getNickname(), "");
				}
				if (!(it->optionalArg.empty()))
				{
					modeWithParamsDone++;
					_modeParamsStr += it->optionalArg + " ";
				}
				trackExecutedModes(it->modeOption, it->mode);
			}
			// Key is already set (ERR_KEYSET)
			catch (const Channel::KeyAlreadySetException &e)
			{
				_serverReply = ErrorReplies::errKeySet(_client->getNickname(), _params[0]);
				_server->handleNewMsgToClient(_serverReply, _client);
				std::cerr << "ERROR: MODE command - A key has already been set" << std::endl;
			}
			catch (const Utils::InvalidPasswordKeyException &e)
			{
				_serverReply =  ":" + serverName + " ERROR: Invalid key format\r\n";
				_server->handleNewMsgToClient(_serverReply, _client);
				std::cerr << "ERROR: MODE command - Invalid key format" << std::endl;
			}
			catch (const CommandMode::InvalidKeyProvidedException &e)
			{
				_serverReply =  ":" + serverName + " ERROR: Invalid key provided\r\n";
				_server->handleNewMsgToClient(_serverReply, _client);
				std::cerr << "ERROR: MODE command - Invalid key provided" << std::endl;
			}
		}
		else if (it->mode == 'o')
		{
			if (modeWithParamsDone == 3)
				continue ;
			try
			{
				// Check if the target client is actually in the channel (ERR_USERNOTINCHANNEL)
				isNicknameInChannel(it->optionalArg, _params[0], false);
				
				if (it->modeOption == '+')
					_channel->giveOperatorPrivileges(_client->getNickname(), it->optionalArg);
				else
					_channel->takeOperatorPrivileges(_client->getNickname(), it->optionalArg);
				modeWithParamsDone++;
				trackExecutedModes(it->modeOption, it->mode);
				_modeParamsStr += it->optionalArg + " ";
			}
			// Target client is not in the channel
			catch(const std::exception& e)
			{
				std::cerr << "ERROR: MODE command - " << e.what() << std::endl;
			}
		}
		else if (it->mode == 'l')
		{
			if (it->modeOption == '+')
			{
				if (modeWithParamsDone == 3)
					continue ;
				int userLimit;
				if (!(Utils::isAllCharactersDigit(it->optionalArg))
					|| !(std::istringstream(it->optionalArg) >> userLimit))
				{
					replyNotNumericOrTooLargeNumber();
					continue ;
				}
				_channel->setIsMaxNbClientsActivated(_client->getNickname(), true);
				_channel->setMaxNbClient(userLimit);
				modeWithParamsDone++;
				_modeParamsStr += it->optionalArg + " ";
			}
			else
				_channel->setIsMaxNbClientsActivated(_client->getNickname(), false);
			trackExecutedModes(it->modeOption, it->mode);
		}
		// Mode unknown from the server (ERR_UNKNOWNMODE)
		else
			replyUnknownModeError(it->mode);
	}
	
	// No modes were executed
	if (_modesStr.empty())
		return ;

	// If there are some mode params, remove the last space in _modeParamsStr
	if (!(_modeParamsStr.empty()))
		_modeParamsStr.erase(_modeParamsStr.end() - 1);
	
	// Broadcast a message to all channel members to inform what was executed
	_serverReply = ":" + _client->getHostmask() + " MODE " + _params[0] + " " + _modesStr + " " + _modeParamsStr + "\r\n";
	_server->broadcastMsgToAllChannelMembers(_channel, _serverReply);

	// Send the RPL_CHANNELMODEIS to the client who issued the command
	_server->handleNewMsgToClient(_channel->buildReplyChannelModeIs(_client->getNickname()), _client);
}

/* ==================================================================================
    * Custom errors
================================================================================== */

void	CommandMode::replyUnknownModeError(char mode)
{
	// convert char to string
	const std::string character(1, mode);

	// Send the error reply (ERR_UNKNOWNMODE)
	_serverReply = ErrorReplies::errUnknownMode(_client->getNickname(), character, _params[0]);
	_server->handleNewMsgToClient(_serverReply, _client);
	
	// Output the error on the terminal
	std::cerr << "ERROR: MODE command - " << character << ": Unknown mode." << std::endl;
}

void	CommandMode::replyNotNumericOrTooLargeNumber(void)
{
	// Send the error reply
	_serverReply = ":" + serverName + "Not numeric parameter or too large number provided (+l) \r\n";
	_server->handleNewMsgToClient(_serverReply, _client);
	
	// Output the error on the terminal
	std::cerr << "ERROR: MODE command - Not numeric parameter or too large number provided (+l)" << std::endl;
}

/* ==================================================================================
    * Function to track all the modes that were successfully executed
================================================================================== */

void		CommandMode::trackExecutedModes(char modeOption, char mode)
{
	_modesStr+= modeOption;
	_modesStr+= mode;
}

/* ==================================================================================
    * Exceptions
================================================================================== */

const char* CommandMode::InvalidKeyProvidedException::what() const throw()
{
	return "Invalid key Provided.";
}
