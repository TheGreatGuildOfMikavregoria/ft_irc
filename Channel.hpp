#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include "Client.hpp"
#include <set>
//#include "Utils.hpp"

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

class Client;

class Channel
{
	private:
		std::string _name;
		std::string _topic;
		std::string _key;
		std::string _timeCreated;

		std::set<Client *> _channelUsers;
		std::set<std::string> _inviteList;
		std::set<std::string> _operators;
				
		bool _inviteOnlyMode = false;
		bool _keyMode = false;
		bool _protectedTopicMode = false;
		bool _clientLimitMode = false;
		size_t _clientLimit = false;

		void _chanOperatorAdd(Client &);
		void _chanOperatorRemove(Client &);
		void _inviteListAdd(std::string &);
		void _inviteListRemove(std::string &);

	public:
		Channel(const std::string &name);
		~Channel() = default;
		void setKey(std::string &);
		void setTopic(std::string &);
		const std::string &getKey() const;
		const std::string &getTopic() const;
		const std::string &getName() const;
		
		std::string &getTimeCreated();
		bool getInviteOnlyMode() const;
		bool getClientLimitMode() const ;
		bool getKeyMode() const;
		bool getProtectedTopicMode() const;
		std::set<Client *> &getChannelUsers();
		std::set<std::string> &getInviteList();
		
		void join(Client &, bool);
		void join(Client &, std::string &key);
		int part(Client &);
		int part(Client &, std::string &message);
		int kick(Client &source, std::string &nick);
		int invite(Client &source, std::string &nick);
		int names(Client &);
		//int topic(Client &source, )
		void chanBroadcast(std::string &);
		void chanBroadcast(Client &, std::string &);
		void userAdd(Client *);
		void userRemove(Client &);
		static bool hasChanPrefix(std::string &name);
		static bool validateName(std::string &name);
};

#endif
