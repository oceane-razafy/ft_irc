/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmourtia <vmourtia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 23:24:49 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/20 16:44:01 by vmourtia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

/* ==================================================================================
    Constructor for a channel object
    -> Check channel name validity based on RFC 2812
    -> Client Creator is by default a member and an operator of the channel
================================================================================== */

Channel::Channel(std::string &channelName, const std::string &creatorName)
    : _creator(creatorName),
    _topic(""),
    _key(""),
    _clients(),
    _invitedClients(),
    _operators(),
    _maxNbClients(20),
    _isMaxNbClientsActivated(false),
    _isInviteOnly(false),
    _isTOPICforOperators(false)
{
     Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] try to create channel", channelName);
        
    /*
        RFC 2812
        1.3 Channels
        2.3.1 Message format in Augmented BNF

        channel    =  ( "#" / "+" / ( "!" channelid ) / "&" ) chanstring
                    [ ":" chanstring ]
        chanstring =  %x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B
        chanstring =/ %x2D-39 / %x3B-FF
                  ; any octet except NUL, BELL, CR, LF, " ", "," and ":" 
    */
   
    // The first character has to be either '&', '#', '+' or '!'
    std::string firstCharacters = "&#+!";
    
    // any octet except BELL, CR, LF, " ", "," and ":"
    std::string forbiddenCharacters = "\x07\r\n ,:";
    
    // The first character has to be either '&', '#', '+' or '!'
    if ((firstCharacters.find(channelName[0]) == std::string::npos)
        // Channel name cannot be longer than 50 characters
        || (channelName.size() > 50)
        // any octet except BELL, CR, LF, " ", "," and ":"
        || (channelName.find_first_of(forbiddenCharacters) != std::string::npos))
        throw InvalidChannelNameException();

    /*
        RFC 2812:
        channel    =  ( "#" / "+" / ( "!" channelid ) / "&" ) chanstring
                    [ ":" chanstring ]
        channelid  = 5( %x41-5A / digit )   ; 5( A-Z / 0-9 )
        
        => For channel name that begins with "!"
        check "channelid  = 5( %x41-5A / digit )   ; 5( A-Z / 0-9 )"
    */
    if (channelName[0] == '!')
    { 
        // It has to have a channelid of 5 characters
        if (channelName.size() < 6)
            throw InvalidChannelNameException();
        
        // Check "channelid  = 5( %x41-5A / digit )   ; 5( A-Z / 0-9 )
        size_t i = 1;
        while (i < 6 && i < channelName.size())
        {
            if (!( isdigit(channelName[i]) || ((channelName[i] >= 'A') && (channelName[i] <= 'Z')) ))
                throw InvalidChannelNameException();
            i++;
        }
    }
    
    // RFC 2812: "Channel names are case insensitive."
	channelName = Utils::toLowerChannelName(channelName);
    _name = channelName;
    
    // Add the creator to the client
    _clients.push_back(_creator);

    // By default, the creator of a channel is an operator
    _operators.push_back(_creator);
}

/* ==================================================================================
    * Getters
================================================================================== */

std::vector<std::string>    &Channel::getClients(void)
{
    return (_clients);
}

std::string Channel::getTopic(void) const
{
    return (_topic);
}

std::string Channel::getName(void) const {
	return _name;
}

/* ==================================================================================
    * Add/Remove a client to/from the channel
================================================================================== */

void    Channel::addClient(const std::string &name)
{
    if (_isMaxNbClientsActivated == true 
        && ((_clients.size() + 1) > static_cast<long unsigned int>(_maxNbClients)))
        throw MaxNbClientsReachedException(); // catch this, and send the appropriate error reply in the command execution
    if (std::find(_clients.begin(), _clients.end(), name) == _clients.end())
        _clients.push_back(name);
    else
        throw AlreadyInChannelException();
    // if the channel was empty before, this new client becomes an operator
    if (_clients.size() == 1 && _operators.size() == 0)
        _operators.push_back(name);
        
    // Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] CHANNEL NAME: ", _name);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] _clients in addClient()", _clients);
}

void    Channel::removeClient(const std::string &name)
{
    std::vector<std::string>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (*it == name)
        {
            _clients.erase(it);
            break ;
        }
    }
    // Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] CHANNEL NAME: ", _name);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] _clients in removeClient()", _clients);
}

/* ==================================================================================
    * Functions to handle clients who have ben invited to the channel
================================================================================== */

// Add the name of the client that has been invited to the channel
void    Channel::addInvitedClients(const std::string &name)
{
    if (std::find(_invitedClients.begin(), _invitedClients.end(), name) == _invitedClients.end())
        _invitedClients.push_back(name);
    // Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] CHANNEL NAME: ", _name);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL]_invitedClients in addInvitedClient()", _invitedClients);
}

// Check if the clients who wants to join has been invited
bool    Channel::isClientInvited(const std::string &name)
{
    // Logger::log(BRIGHT_ORANGE, "[CHANNEL] in isClientInvited");
    // Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] CHANNEL NAME: ", _name);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] _invitedClients in isClientInvited()", _invitedClients);
    std::vector<std::string>::iterator it;
    for (it = _invitedClients.begin(); it != _invitedClients.end(); it++)
    {
        if (*it == name)
            return (true);
    }
    return (false);
}

// Remove the invited clients from this vector, when the client joins the channel
void    Channel::removeClientInvited(const std::string &name)
{
    std::vector<std::string>::iterator it;
    for (it = _invitedClients.begin(); it != _invitedClients.end(); it++)
    {
        if (*it == name)
        {
            _invitedClients.erase(it);
            break ;
        }
    }
    // Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] CHANNEL NAME: ", _name);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] _invitedClients after removeClientInvited()", _invitedClients);
}   

/* ==================================================================================
    * If a client wants to join a channel that has a key,
    check if the key provided matches the actual channel key
================================================================================== */

bool    Channel::isChannelKey(const std::string &key)
{
    if (_key == key)
        return (true);
    return (false);
}

/* ==================================================================================
    * To check if the channel is invite-only or not
================================================================================== */

bool    Channel::getIsInviteOnly(void) const
{
    return (_isInviteOnly);
}

/* ==================================================================================
    * KICK - Eject a client from the channel
================================================================================== */

void    Channel::ejectClient(const std::string &clientWhoAsked, const std::string &clientToEject)
{
    // Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] CHANNEL NAME: ", _name);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] before ejectClient() -> _clients", _clients);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] before ejectClient() -> _operators", _operators);
    
   checkClientChannelRights(clientWhoAsked, true, true);
   
   // remove the clientToEject from the channel
   removeClient(clientToEject);
   removeOperator(clientToEject);

    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] after ejectClient() -> _clients", _clients);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] after ejectClient() -> _operators", _operators);
}

/* ==================================================================================
    ∗ TOPIC - Change or view the channel topic
    ∗ MODE - Change the channel’s mode:
           t: Set/remove the restrictions of the TOPIC command to channel operators
================================================================================== */

// To change the topic
void    Channel::setTopic(const std::string &clientName, const std::string &topic)
{
    checkClientRightsForTOPIC(clientName); 
    _topic = topic;
}

// To view the topic
std::string Channel::viewTopic(const std::string &clientName)
{
    checkClientRightsForTOPIC(clientName);
    return (_topic);
}

// To set/remove the restrictions of the TOPIC command to channel operators
void    Channel::setIsTOPICforOperators(const std::string &clientName, bool isTOPICforOperator)
{
    checkClientChannelRights(clientName, true, true);
    _isTOPICforOperators = isTOPICforOperator;
}

void    Channel::checkClientRightsForTOPIC(const std::string &clientName)
{
    // if TOPIC command is only for operators
    // check if the client is a member and then an operator of this channel
    if (_isTOPICforOperators == true)
        checkClientChannelRights(clientName, true, true);  

    // if TOPIC command is not only for operators,
    // just check if the client is a member of this channel
    else
        checkClientChannelRights(clientName, true, false); 
}

/* ==================================================================================
    ∗ MODE
        i: Set/remove Invite-only channel
================================================================================== */
/*
    - setIsInviteOnly(true) -> channel will be Invite-only
    - setIsInviteOnly(false) -> channel will not be Invite-only
*/
void    Channel::setIsInviteOnly(const std::string &clientName, bool isInviteOnly)
{
    checkClientChannelRights(clientName, true, true);  
    _isInviteOnly = isInviteOnly;
}

/* ==================================================================================
    ∗ MODE
        k: Set/remove the channel key (password)
    * JOIN with a key
================================================================================== */

// Set/remove the key
// NOTE: To remove the key, set it to empty string ("")
void    Channel::setKey(const std::string &clientName, const std::string &key)
{
    // Check if client is a member and an operator of this channel
    checkClientChannelRights(clientName, true, true);

    // If the channel key is already not empty
    // and the key that the client wants to set is not empty 
    // (which means he wants to change the key)
    // -> throw exception key has already been set
    if (!(_key.empty()) 
        && !(key.empty()))
        throw KeyAlreadySetException();

    // Check the format of the key
    if (!(key.empty()))
        Utils::checkPasswordKeyValidity(key);
    
    // Set/remove the key
    _key = key;
    if (!(_key.empty()))
        Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] A key has been set:", _key);
}

// Check if a key is set when JOIN command is executed
bool Channel::isKeySet(void) const
{
	return !_key.empty(); 
}

/* ==================================================================================
    ∗ MODE
        o: Give/take channel operator privilege
================================================================================== */

void    Channel::addOperator(const std::string &name)
{
    if (std::find(_operators.begin(), _operators.end(), name) == _operators.end())
        _operators.push_back(name);
        
    // Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] CHANNEL NAME: ", _name);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] _operators after addOperators()", _operators);
}

// When a client wants to remove operator privileges from another target client
void    Channel::giveOperatorPrivileges(const std::string &clientWhoAsked, const std::string &clientToMakeChanOp)
{
    checkClientAndTargetRights(clientWhoAsked, clientToMakeChanOp);
    addOperator(clientToMakeChanOp);
}

void    Channel::removeOperator(const std::string &name)
{
    std::vector<std::string>::iterator it;
    for (it = _operators.begin(); it != _operators.end(); it++)
    {
        if (*it == name)
        {
            _operators.erase(it);
            // If there is no operator anymore after removing this one
            // and there are still clients in the channel,
            // by default, the second who entered the channel becomes an operator
            if (_operators.size() == 0 && _clients.size() != 0)
                _operators.push_back(_clients[0]);
            break ;
        }
    }
    
    // Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] CHANNEL NAME: ", _name);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] _operators after removeOperators()", _operators);
}

// When a client wants to give operator privileges to another target client
void    Channel::takeOperatorPrivileges(const std::string &clientWhoAsked, const std::string &clientToMakeChanOp)
{
    checkClientAndTargetRights(clientWhoAsked, clientToMakeChanOp);
    removeOperator(clientToMakeChanOp);
}

/* ==================================================================================
    ∗ MODE
        l: Set/remove the user limit to channel
================================================================================== */

void    Channel::setIsMaxNbClientsActivated(const std::string &clientName, bool isMaxNbClientsActivated)
{
    checkClientChannelRights(clientName, true, true);
    _isMaxNbClientsActivated = isMaxNbClientsActivated;
}

void    Channel::setMaxNbClient(int maxNbClients)
{
    _maxNbClients = maxNbClients;
}

/* ==================================================================================
    Functions to check the rights of the clients
================================================================================== */

// To check if the client is a member of the channel
bool    Channel::isMember(const std::string &name)
{
    // Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] CHANNEL NAME: ", _name);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] _clients in isMember()", _clients);
    
    std::vector<std::string>::iterator it;
    for (it = _clients.begin(); it < _clients.end(); it++)
    {
        if (*it == name)
            return (true);
    }
    return (false);
}

// To check if the client is an operator of the channel
bool    Channel::isOperator(const std::string &name)
{
    // Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] CHANNEL NAME: ", _name);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] _operators in isOperator()", _operators);
    
    std::vector<std::string>::iterator it;
    for (it = _operators.begin(); it < _operators.end(); it++)
    {
        if (*it == name)
            return (true);
    }
    return (false);
}

/* 
    ==> To check if the client is a member, and then an operator of the channel
    
    NOTE: Check both cases, as error replies from server will be different
    442  ERR_NOTONCHANNEL
    441  ERR_USERNOTINCHANNEL
    482  ERR_CHANOPRIVSNEEDED
    485  ERR_UNIQOPPRIVSNEEDED
*/
void    Channel::checkClientChannelRights(const std::string &name, bool checkIsMember, bool checkIsOperator)
{
        // The client is not a member of the channel
    if (checkIsMember == true && isMember(name) == false)
        throw ClientNotAMemberOfThisChannelException();

    // The client is not an operator of the channel
    if (checkIsOperator == true && isOperator(name) == false)
        throw ClientNotAnOperatorOfThisChannelException();
}

/* 
    ==> To check both client and target rights
    
    NOTE: Check all thoses cases, as error replies from server will be different
    441  ERR_USERNOTINCHANNEL
    442  ERR_NOTONCHANNEL
    482  ERR_CHANOPRIVSNEEDED
    485  ERR_UNIQOPPRIVSNEEDED
*/
void    Channel::checkClientAndTargetRights(const std::string &clientName, const std::string &targetName)
{
    // Check if the client who issued the command
    // is a member, and then a operator of the channel
    try
    {
        checkClientChannelRights(clientName, true, true);
    }
    catch (const std::exception &e)
    {
        throw ;
    }
    
    // Check if the target client is a member of the channel
    try 
    {
        checkClientChannelRights(targetName, true, false);
    }
    catch (const std::exception &e)
    {
        throw TargetNotAMemberOfTheChannelException();
    }
}

/* ==================================================================================
    Function to build the reply RPL_CHANNELMODEIS
================================================================================== */

std::string Channel::buildReplyChannelModeIs(const std::string &nickname) const
{
    std::string mode = "+";
    std::string modeParams;
    
    if (_isInviteOnly)
        mode += "i";
    if (_isTOPICforOperators)
        mode += "t";
    if (!(_key.empty()))
    {
        mode += "k";
        modeParams += _key;
    }
    if (!(_operators.empty()))
    {
        for (size_t i = 0; i < _operators.size(); i++)
        {
            mode += "o";
            if (!(modeParams.empty()))
                modeParams += " ";
            modeParams += _operators[i]; 
        }
    }
    if (_isMaxNbClientsActivated)
    {
        std::stringstream ss;
        ss << _maxNbClients;
        mode += "l";
        if (!(modeParams.empty()))
            modeParams += " ";
        modeParams += ss.str();
    }
    return (CommandResponses::rplChannelModeIs(nickname, _name, mode, modeParams)); 
}

/* ==================================================================================
    Helper to build the reply RPL_NAMREPLY
================================================================================== */

std::string Channel::getListOfAllUsersNamesInChannelAsStr(void)
{
	std::string list;
	std::vector<std::string>::iterator it = _clients.begin();
	for ( ; it != _clients.end(); ++it )
	{
		if (isOperator(*it))
			list += "@";
		// else
		// 	list += "+"; // only for voices members (not in the subject)
		list += (*it + " ");
	}
	if (list.size() > 0)
		list.erase(list.size() - 1);
	return list;
    
    // Logger::logValue(BRIGHT_ORANGE, "[CHANNEL] CHANNEL NAME: ", _name);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] _clients", _clients);
    // Logger::logVector(BRIGHT_ORANGE, "[CHANNEL] _operators", _operators);
}

/* ==================================================================================
    Change nicknames
================================================================================== */

void Channel::_changeNicknameInVector(std::vector<std::string> &vect, std::string newNickname, std::string oldNickname)
{
	if (vect.empty())
		return ;
	std::vector<std::string>::iterator itClient;
	itClient = std::find(vect.begin(), vect.end(), oldNickname);
	if (itClient != vect.end())
		*itClient = newNickname;
}

void Channel::changeThisNickname(std::string newNickname, std::string oldNickname)
{
	_changeNicknameInVector(_clients, newNickname, oldNickname);
	_changeNicknameInVector(_operators, newNickname, oldNickname);
	_changeNicknameInVector(_invitedClients, newNickname, oldNickname);
}

/* ==================================================================================
    * Exceptions thrown in this Channel class
================================================================================== */

const char *Channel::InvalidChannelNameException::what() const throw()
{
	return ("ERROR: Channel name is invalid");
}

const char *Channel::MaxNbClientsReachedException::what() const throw()
{
	return ("ERROR: Maximum number of clients has been reached in the channel");
}

const char *Channel::ClientNotAMemberOfThisChannelException::what() const throw()
{
	return ("ERROR: The client is not a member of this channel");
}

const char *Channel::ClientNotAnOperatorOfThisChannelException::what() const throw()
{
	return ("ERROR: The client is not an operator of this channel");
}

const char *Channel::KeyAlreadySetException::what() const throw()
{
	return ("ERROR: The key as already been set");
}

const char *Channel::TargetNotAMemberOfTheChannelException::what() const throw()
{
	return ("ERROR: The target client is not a member of the channel");
}

const char *Channel::AlreadyInChannelException::what() const throw()
{
	return ("ERROR: Cannot join twice: the client is already a member of the channel.");
}
