#pragma once

#include "Server.hpp"
#include "Utils.hpp"

#include <string>
#include <iostream>
#include <sstream>

class Client;

inline std::string stringify(const std::string& s) { return s; }
inline std::string stringify(const char* s) { return std::string(s); }
inline std::string stringify(char* s) { return std::string(s); }
inline std::string stringify(char ch) { return std::string(1, ch); }

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
}

#define RPL_WELCOME				" 001 %s :Welcome to the %s Network, %s!%s@%s" 
#define RPL_YOURHOST			" 002 %s :Your host is %s, running version %s" 
#define RPL_CREATED				" 003 %s :This server was created %s" 
#define RPL_MYINFO				" 004 %s %s %s %s %s %s" 
#define RPL_ISUPPORT			" 005 %s %s :are supported by this server" 
#define RPL_UMODEIS				" 221 %s %s"  
#define RPL_ENDOFWHO			" 315 %s %s :End of WHO list" 
#define RPL_CHANNELMODEIS		" 324 %s %s %s %s" 
#define RPL_CREATIONTIME		" 329 %s %s %s" 
#define RPL_NOTOPIC				" 331 %s %s :No topic is set" 
#define RPL_TOPIC				" 332 %s %s :%s" 
#define RPL_TOPICWHOTIME		" 333 %s %s %s %s" 
#define RPL_INVITING			" 341 %s %s %s" 
#define RPL_WHOREPLY			" 352 %s %s %s %s %s %s %s :%s %s" 
#define RPL_NAMREPLY			" 353 %s %s %s :%s" 
#define RPL_ENDOFNAMES			" 366 %s %s :End of /NAMES list" 
#define RPL_YOUREOPER			" 381 %s :You are now an IRC operator" 
#define ERR_NOSUCHNICK			" 401 %s %s :No such nick/channel" 
#define ERR_NOSUCHCHANNEL		" 403 %s %s :No such channel" 
#define ERR_CANNOTSENDTOCHAN	" 404 %s %s :Cannot send to channel" 
#define ERR_TOOMANYTARGETS		" 407 %s %s :Too many targets" 
#define ERR_NOORIGIN			" 409 %s :No origin specified" 
#define ERR_NORECIPIENT			" 411 %s :No recipient given (%s)" 
#define ERR_NOTEXTTOSEND		" 412 %s :No text to send" 
#define ERR_NOMOTD				" 422 %s :MOTD File is missing" 
#define ERR_UNKNOWNCOMMAND		" 421 %s %s :Unknown command" 
#define ERR_NONICKNAMEGIVEN		" 431 %s :No nickname given" 
#define ERR_ERRONEUSNICKNAME	" 432 %s %s :Erroneus nickname" 
#define ERR_NICKNAMEINUSE		" 433 %s %s :Nickname is already in use" 
#define ERR_USERNOTINCHANNEL	" 441 %s %s %s :They aren't on that channel" 
#define ERR_NOTONCHANNEL		" 442 %s %s :You're not on that channel" 
#define ERR_USERONCHANNEL		" 443 %s %s %s :is already on channel" 
#define ERR_NOTREGISTERED		" 451 %s :You have not registered" 
#define ERR_NEEDMOREPARAMS		" 461 %s %s :Not enough parameters" 
#define ERR_ALREADYREGISTERED	" 462 %s :You may not reregister" 
#define ERR_PASSWDMISMATCH		" 464 %s :Password incorrect" 
#define ERR_CHANNELISFULL		" 471 %s %s :Cannot join channel (+l)" 
#define ERR_UNKNOWNMODE			" 472 %s %s :is unknown mode char to me" 
#define ERR_INVITEONLYCHAN		" 473 %s %s :Cannot join channel (+i)" 
#define ERR_BADCHANNELKEY		" 475 %s %s :Cannot join channel (+k)" 
#define ERR_BADCHANMASK			" 476 %s %s :Bad Channel Mask" 
#define ERR_CHANOPRIVSNEEDED	" 482 %s %s :You're not channel operator" 
#define ERR_NOOPERHOST			" 491 %s :No O-lines for your host" 
#define ERR_UMODEUNKNOWNFLAG	" 501 %s :Unknown MODE flag" 
#define ERR_USERSDONTMATCH		" 502 %s :Cant change mode for other users" 
#define NTC_INVALIDUSRNAME		" NOTICE %s :*** Invalid username. Username must contain only alphanumeric characters" 
