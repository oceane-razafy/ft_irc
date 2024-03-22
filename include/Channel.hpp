/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmourtia <vmourtia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 21:07:26 by orazafy           #+#    #+#             */
/*   Updated: 2024/03/20 16:44:31 by vmourtia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "Logger.hpp"
# include "Client.hpp"
# include "CommandResponses.hpp"
# include "Utils.hpp"
# include <string>
# include <vector>
# include <algorithm>
# include <sstream>
# include <cctype>

class Channel
{
	public: 
                         
                                    Channel(std::string &channelName, const std::string &creatorName);

    // Getters
    std::vector<std::string>        &getClients(void);
	std::string                     getTopic(void) const;
    std::string                     getName(void) const;

    // Add/remove a client to/from the channel
    void                            addClient(const std::string &name);
    void                            removeClient(const std::string &name);

    // Functions to handle clients who have ben invited to the channel
    void                            addInvitedClients(const std::string &name);
    bool                            isClientInvited(const std::string &name);
    void                            removeClientInvited(const std::string &name);                                            

    // Check if the key provided by the client matches the actual channel key
    bool                            isChannelKey(const std::string &key);

    // To check if the channel is invite-only or not
    bool                            getIsInviteOnly(void) const;

    // For KICK command
    void                            ejectClient(const std::string &clientWhoAsked, const std::string &clientToEject);
     
    // For TOPIC
    // For MODE -/+ t
    void                            setTopic(const std::string &clientName, const std::string &topic);
    std::string                     viewTopic(const std::string &clientName);
    void                            setIsTOPICforOperators(const std::string &clientName, bool isTOPICforOperator);
    void                            checkClientRightsForTOPIC(const std::string &clientName);

    // For MODE -/+i
    void                            setIsInviteOnly(const std::string &clientName, bool isInviteOnly);
    
    // For MODE -/+ k
    // For JOIN with a key
    void                            setKey(const std::string &clientName, const std::string &key);
	bool                            isKeySet(void) const;
    
    // For MODE -o
    void                            addOperator(const std::string &name);
    void                            giveOperatorPrivileges(const std::string &clientWhoAsked, const std::string &clientToMakeChanOp);
    void                            removeOperator(const std::string &name);
    void                            takeOperatorPrivileges(const std::string &clientWhoAsked, const std::string &clientToMakeChanOp);

    // For MODE -l
    void                            setIsMaxNbClientsActivated(const std::string &clientName, bool isMaxNbClientsActivated);
    void                            setMaxNbClient(int maxNbClients);
    
    // Functions to check the rights of the clients
    bool                            isMember(const std::string &name);
    bool                            isOperator(const std::string &name);
    void                            checkClientChannelRights(const std::string &name, bool checkIsMember, bool checkIsOperator);
    void                            checkClientAndTargetRights(const std::string &clientName, const std::string &targetName);

    // Function to build the reply RPL_CHANNELMODEIS
    std::string                     buildReplyChannelModeIs(const std::string &nickname) const;

    //  Helper to build the reply RPL_NAMREPLY
	std::string                     getListOfAllUsersNamesInChannelAsStr(void);

	void							changeThisNickname(std::string newNickname, std::string oldNickname);

    // Exceptions
    class InvalidChannelNameException : public std::exception
    {
        public:
            virtual const char* what() const throw();
    };

    class MaxNbClientsReachedException : public std::exception
    {
        public:
            virtual const char* what() const throw();
    };

    class ClientNotAMemberOfThisChannelException : public std::exception
    {
        public:
            virtual const char* what() const throw();
    };

    class ClientNotAnOperatorOfThisChannelException : public std::exception
    {
        public:
            virtual const char* what() const throw();
    };

    class KeyAlreadySetException : public std::exception
    {
        public:
            virtual const char* what() const throw();
    };

    class  TargetNotAMemberOfTheChannelException : public std::exception
    {
        public:
            virtual const char* what() const throw();
    };

    class  AlreadyInChannelException : public std::exception
    {
        public:
            virtual const char* what() const throw();
    };

    private:


        std::string                 _name;
        std::string                 _creator; 
        std::string                 _topic;
        std::string                 _key;
        
        std::vector<std::string>    _clients;
        std::vector<std::string>    _invitedClients;
        std::vector<std::string>    _operators;

        int                         _maxNbClients; // For MODE -/+l
        bool                        _isMaxNbClientsActivated; // For MODE -/+l
        bool                        _isInviteOnly; // for MODE -/+i
        bool                        _isTOPICforOperators; // for MODE -/+t

		void						_changeNicknameInVector(std::vector<std::string> &vect, std::string newNickname, std::string oldNickname);
};

#endif