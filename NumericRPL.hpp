#pragma once

#include "Server.hpp"

#include <string>
#include <iostream>
#include <sstream>
//#include "Client.hpp" 

class Client;

inline std::string stringify(const std::string& s) { return s; }
inline std::string stringify(const char* s) { return std::string(s); }
inline std::string stringify(char* s) { return std::string(s); }

template<typename T>
std::string stringify(T val) { return std::to_string(val); }

inline void buildResponse(std::string& reply, const char* format) {
	reply += format;
}

template<typename T, typename... Args>
void buildResponse(std::string& reply, const char* format, T value, Args... args) {
	while (*format) {
		if (*format == '%' && *(format + 1) == 's') {
			reply += stringify(value);
			format += 2;
			buildResponse(reply, format, args...);
			return;
		}
		reply += *format++;
	}
}

template<typename... Args>
std::string numericRPL(const std::string& format, Args... args) {
	std::string reply = ":ircserv"; 
	buildResponse(reply, format.c_str(), args...);
	if (reply.length() < 2 || reply.substr(reply.length() - 2) != "\r\n") {
		reply += "\r\n";
	}
	return (reply);
	// c.getOutBuf().append(reply.c_str(), reply.length());
}
//Connection Messages
	//CAP message
	//AUTHENTICATE message
	//PASS message		461	462	464
	//NICK message		431	432	433	436
	//USER message		461	462
	//PING message		461	409	402
	//OPER message		461	464	491	491
	//QUIT message
	//ERROR message

//Channel Operations
	//JOIN message		332	333	353	366	405	461	403	405	475	474	471	473	476	332	333	353	366
	//PART message		442	403	461	403	442
	//TOPIC message		442	482	461	403	442	482	331	332	333
	//NAMES message		353	366
	//LIST message		321	322	323
	//INVITE message	341	461	403	442	482	443	336	337
	//KICK message		461	403	482	441	442	476

//Server Queries and Commands
	//MODE message		401	502	221	501	403	324	329	482	367	368	348	349	336	337

//Sending Messages
	//PRIVMSG message	404	301	401	402	404	407	411	412	413	414	301
	//NOTICE message

//User-Based Queries
	//WHO message		352	315
	//WHOIS message		318	401	402	431	276	307	311	312	313	317	319	320	330	338	378	379	671	301

//Operator Messages
	//KILL message		723	402	461	481	723

//Optional Messages
	//AWAY message		306	305	301	302	352	305	306

//Unimplemented Messages
	//MOTD message
	//VERSION Message
	//ADMIN message
	//CONNECT message
	//LUSERS message
	//TIME message
	//STATS message
	//HELP message
	//INFO message
	//WHOWAS message
	//REHASH message
	//RESTART message
	//SQUIT message
	//LINKS message
	//USERHOST message
	//WALLOPS message

#define RPL_WELCOME				" 001 %s :Welcome to the %s Network, %s!%s@%s" //in_use
#define RPL_YOURHOST			" 002 %s :Your host is %s, running version %s" //in_use
#define RPL_CREATED				" 003 %s :This server was created <datetime>" //in_use //Add params
#define RPL_MYINFO				" 004 %s <servername> <version> <available user modes> <available channel modes> [<channel modes with a parameter>]" //in_use //Add params
#define RPL_ISUPPORT			" 005 %s <1-13 tokens> :are supported by this server" //in_use //Add params
#define RPL_UMODEIS				" 221 %s <user modes>"
#define RPL_WHOISCERTFP			" 276 %s <nick> :has client certificate fingerprint <fingerprint>" //what is fingerprint
#define RPL_AWAY				" 301 %s <nick> :<message>"
#define RPL_USERHOST			" 302 %s :[<reply>{ <reply>}]" 
#define RPL_UNAWAY				" 305 %s :You are no longer marked as being away"
#define RPL_NOWAWAY				" 306 %s :You have been marked as being away"
#define RPL_WHOISREGNICK		" 307 %s <nick> :has identified for this nick"
#define RPL_WHOISUSER			" 311 %s <nick> <username> <host> * :<realname>"
#define RPL_WHOISSERVER			" 312 %s <nick> <server> :<server info>"
#define RPL_WHOISOPERATOR		" 313 %s <nick> :is an IRC operator"
#define RPL_ENDOFWHO			" 315 %s <mask> :End of WHO list"
#define RPL_WHOISIDLE			" 317 %s <nick> <secs> <signon> :seconds idle, signon time"
#define RPL_ENDOFWHOIS			" 318 %s <nick> :End of /WHOIS list"
#define RPL_WHOISCHANNELS		" 319 %s <nick> :[prefix]<channel>{ [prefix]<channel>}" //read more on format
#define RPL_WHOISSPECIAL		" 320 %s <nick> :blah blah blah"
#define RPL_LISTSTART			" 321 %s Channel :Users  Name"
#define RPL_LIST				" 322 %s <channel> <client count> :<topic>"
#define RPL_LISTEND				" 323 %s :End of /LIST"
#define RPL_CHANNELMODEIS		" 324 %s <channel> <modestring> <mode arguments>..." //see what are mode args
#define RPL_CREATIONTIME		" 329 %s <channel> <creationtime>"
#define RPL_WHOISACCOUNT		" 330 %s <nick> <account> :is logged in as"
#define RPL_NOTOPIC				" 331 %s %s :No topic is set" //in_use
#define RPL_TOPIC				" 332 %s %s :%s" // in_use
#define RPL_TOPICWHOTIME		" 333 %s %s %s %s" //in_use
#define RPL_INVITELIST			" 336 %s <channel>"
#define RPL_ENDOFINVITELIST		" 337 %s :End of /INVITE list"
#define RPL_WHOISACTUALLY (338) //read more on format
#define RPL_INVITING			" 341 %s <nick> <channel>"
#define RPL_EXCEPTLIST			" 348 %s <channel> <mask>"
#define RPL_ENDOFEXCEPTLIST		" 349 %s <channel> :End of channel exception list"
#define RPL_WHOREPLY			" 352 %s <channel> <username> <host> <server> <nick> <flags> :<hopcount> <realname>"
#define RPL_NAMREPLY			" 353 %s %s %s :%s" // in_use
#define RPL_ENDOFNAMES			" 366 %s %s :End of /NAMES list" //in_use
#define RPL_BANLIST				" 367 %s <channel> <mask> [<who> <set-ts>]"
#define RPL_ENDOFBANLIST		" 368 %s <channel> :End of channel ban list"
#define RPL_WHOISHOST			" 378 %s <nick> :is connecting from *@localhost 127.0.0.1"
#define RPL_WHOISMODES			" 379 %s <nick> :is using modes +ailosw"
#define RPL_YOUREOPER			" 381 %s :You are now an IRC operator" //in_use
#define ERR_NOSUCHNICK			" 401 %s <nickname> :No such nick/channel"
#define ERR_NOSUCHSERVER		" 402 %s <server name> :No such server" //PING:Deprecated Numeric Reply
#define ERR_NOSUCHCHANNEL		" 403 %s %s :No such channel" // in_use
#define ERR_CANNOTSENDTOCHAN	" 404 %s <channel> :Cannot send to channel"
#define ERR_TOOMANYCHANNELS		" 405 %s <channel> :You have joined too many channels"
#define ERR_TOOMANYTARGETS (407)//no message?
#define ERR_NOORIGIN			" 409 %s :No origin specified" //in_use
#define ERR_NORECIPIENT			" 411 %s :No recipient given (<command>)"
#define ERR_NOTEXTTOSEND		" 412 %s :No text to send"
#define ERR_NOTOPLEVEL (413)	//no message?
#define ERR_WILDTOPLEVEL (414)	//no message?
#define ERR_NONICKNAMEGIVEN		" 431 %s :No nickname given" //in_use
#define ERR_ERRONEUSNICKNAME	" 432 %s %s :Erroneus nickname" //in_use
#define ERR_NICKNAMEINUSE		" 433 %s :Nickname is already in use" //in_use
//#define ERR_NICKCOLLISION		" 436 %s <nick> :Nickname collision KILL from <user>@<host>" //out of scope. Involves another server
#define ERR_USERNOTINCHANNEL	" 441 %s <nick> <channel> :They aren't on that channel"
#define ERR_NOTONCHANNEL		" 442 %s %s :You're not on that channel" //in_use
#define ERR_USERONCHANNEL		" 443 %s %s %s :is already on channel" //in_use
#define ERR_NOTREGISTERED 	" 451 %s :You have not registered" //in_use
#define ERR_NEEDMOREPARAMS		" 461 %s %s :Not enough parameters" //in_use
#define ERR_ALREADYREGISTERED	" 462 %s :You may not reregister" //in_use
#define ERR_PASSWDMISMATCH		" 464 %s :Password incorrect" //in_use
#define ERR_CHANNELISFULL		" 471 %s %s :Cannot join channel (+l)" // in_use
#define ERR_INVITEONLYCHAN		" 473 %s %s :Cannot join channel (+i)" //in_use
#define ERR_BANNEDFROMCHAN		" 474 %s <channel> :Cannot join channel (+b)"
#define ERR_BADCHANNELKEY		" 475 %s %s :Cannot join channel (+k)" //in_use
#define ERR_BADCHANMASK			" 476 %s %s :Bad Channel Mask" //in_use
#define ERR_NOPRIVILEGES		" 481 %s :Permission Denied- You're not an IRC operator"
#define ERR_CHANOPRIVSNEEDED	" 482 %s %s :You're not channel operator" // in_use
#define ERR_NOOPERHOST			" 491 %s :No O-lines for your host" //in_use
#define ERR_UMODEUNKNOWNFLAG	" 501 %s :Unknown MODE flag"
#define ERR_USERSDONTMATCH		" 502 %s :Cant change mode for other users"
#define RPL_WHOISSECURE			" 671 %s <nick> :is using a secure connection"
#define ERR_NOPRIVS				" 723 %s <priv> :Insufficient oper privileges."
