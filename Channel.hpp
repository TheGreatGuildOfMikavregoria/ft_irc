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
	i   --- only users from the invite list can join the channel (D)
	t   --- topic can be set only by an operator (D)
	k   --- check key when joining the channel (B)
	o   --- user mode to set to operator (B)
	l   --- client limit - if on (C)
*/

class Client;

class Channel
{
	private:
		std::string _name;
		std::string _topic;
		std::time_t _topicUpdatedTime;
		std::string _topicUpdatedWho;
		std::string _key;
		std::string _timeCreated;

		std::set<Client *>		_channelUsers;
		std::set<std::string>	_inviteList;
		std::set<std::string>	_operators;
		

		// bool _inviteOnlyMode = false;
		// bool _keyMode = false;
		// bool _protectedTopicMode = false;
		// bool _clientLimitMode = false;
		int		_chanMode		= 0;
		size_t	_clientLimit	= 0;


		void _inviteListAdd(std::string &);
		void _inviteListRemove(std::string &);

	public:
		enum mode {
			ModeNone			= 0,
			ModeInviteOnly		= 1 << 0,
			ModeProtectedTopic	= 1 << 1,
			ModeKeyOn			= 1 << 2,
			ModeClientLim		= 1 << 3,
			ModeOper			= 1 << 4
		};
		Channel(const std::string &name);
		~Channel() = default;
		void setKey(std::string &);
		void setTopic(std::string &);
		void setClientLimit(size_t);
		const std::string &getKey() const;
		const std::string &getTopic() const;
		const std::string &getName() const;
		
		const std::string &getTimeCreated();
		bool getInviteOnlyMode() const;
		bool getClientLimitMode() const ;
		bool getKeyMode() const;
		bool getProtectedTopicMode() const;
		size_t getClientLimit() const;
		std::set<Client *> &getChannelUsers();
		std::set<std::string> &getInviteList();
		
		void join(Client &, bool);
		void join(Client &, std::string &key);
		void part(Client &, std::string &message);
		int kick(Client &source, std::string &nick);
		bool invite(Client &source, std::string &nick);
		void names(Client &);
		void topic(Client &source, bool);
		void topic(Client &source, std::string &newTopic);
		void chanBroadcast(std::string &);
		void chanBroadcast(Client &, std::string &);
		void userAdd(Client *);
		void userRemove(Client &);
		static bool hasChanPrefix(std::string &name);
		static bool validateName(std::string &name);
		
		void	addMode(int mask);
		void	removeMode(int mask);
		bool	hasMode(int mask) const;
		const 	std::string	getChanMode() const;
		void 	chanOperatorAdd(std::string &nick);
		bool 	chanOperatorRemove(std::string &nick);

		bool	isOperator(Client &user);
//		const	std::string	getChanModeParams() const;
};

#endif
