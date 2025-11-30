#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include "Client.hpp"

/*
Directly Associated Commands
	JOIN 
	PART
	TOPIC
	NAMES
	LIST
	INVITE
	KICK

Indirectly associated Commands
	PRIVMSG
	QUIT

must have modes:
	i   --- only users from the invite list can join the channel
	t   --- topic can be set only by an operator
	k   --- check key when joining the channel
	o   --- user mode to set to operator
	l   --- client limit - if on
*/

class Channel
{
	private:
		std::string _name;
		std::string _topic;
		std::string _key;

		std::vector<User> _channelUsers;
		std::vector<std::string> _inviteList;
				
		bool _inviteOnlyMode;
		bool _clientLimitMode;
		bool _keyMode;
		bool _protectedTopicMode;
		

	public:
		Channel(std::string name)
		~Channel();
		void setKey(std::string &);
		void setTopic(std::string &);
		const std::string &getKey() const;
		const stg::string &getTopic() const;
		const std::string &getName() const;
		
		bool getInviteOnlyMode() const;
		bool getClientLimitMode() const ;
		bool getKeyMode() const;
		bool getProtectedTopicMode() const;
		std::vector<User> &getChannelUsers();
		std::vector<std::string> &getInviteList();
		
//basic, status for return
		int join(Client &);
//with key
		int join(Client &, std::string &);
		int part(Client &);

		
		static bool validateName(std::string &name);
//		addUser()
		
		
}

#endif
