#include "../Server.hpp"
// #define VALID_USER_MODES "iows"
// #define VALID_CHAN_MODES "itkol"
// #define MODE_CHAN 1
// #define MODE_USER 0
#define ADD_MODE 1
#define REMOVE_MODE 0

bool	Server::isValidModeString(const std::string& modeString){
	if (modeString.size() < 2 || !(modeString[0] == '+' || modeString[0] == '-'))
		return false;
	// int  signCount = 1;
	// std::string validModeSet = whichMode ? VALID_CHAN_MODES : VALID_USER_MODES; //this might be extra. remove it.
	for (size_t i = 1; i < modeString.size(); ++i) {
		char ch = modeString[i];
		if (ch == '+' || ch == '-') {
			// if (++signCount > 1 || i == modeString.size() - 1)
			// 	return false;
			continue;
		}
			// signCount = 0;
			// if (validModeSet.find(ch) == std::string::npos) {
		// 	// 	return false;
		// }
	}
	return true;
}

bool 	Server::updateChanKey(Channel* chan, std::string& key) {
	for (char ch : key){
		if (!std::isprint(static_cast<unsigned char>(ch)) || std::isspace(static_cast<unsigned char>(ch)))
			return false;
	}
	chan->setKey(key);
	return true;	
}

bool	Server::updateChanOper(Client& c, Channel* chan, std::string& newOperName) {
	const std::string nickName = c.getNickName();
	std::string rpl;
	Buffer &outBuf = c.getOutBuf();
	Client* newOper = clientLookUp(newOperName);
	if (!newOper) {
		rpl = numericRPL(ERR_NOSUCHNICK, nickName, newOperName);
		outBuf.append(rpl.c_str(), rpl.length());
		return false;
	}
	else if (!chan->getChannelUsers().count(newOper)) {
		rpl = numericRPL(ERR_USERNOTINCHANNEL, nickName, newOperName, chan->getName());
		outBuf.append(rpl.c_str(), rpl.length());
		return false;
	}
	chan->chanOperatorAdd(newOperName);
	return true;
}

bool Server::updateChanULimit(Channel* chan, const std::string& strLim)
{
    char* endPtr;
    long val = std::strtol(strLim.c_str(), &endPtr, 10);

    if (*endPtr != '\0') return false;
    if (val <= 0) return false;
    if (val > INT32_MAX) return false; //change the macro to INT_MAX if it's available on school computer. hpp has the library

    chan->setClientLimit(static_cast<size_t>(val));
    return true;
}


//if mode arguments are not enough still the valid modes should be processes.
//liberschat writes channels modes fore MODE command even if the client is not a member. but shouldn't let client change the modes.
std::string	Server::applyChanMode(Client& c, Channel* chan, Command& cmd) {
	const std::string nickName = c.getNickName();
	Buffer &outBuf = c.getOutBuf();
	std::string target = cmd.getTokens().at(1);
	unsigned long argID = cmd.getTokens().size() >= 4 ? 3 : 0;
	std::string rpl;
	std::string rplRet;
	std::string validModeAdd;
	std::string validModeRem;
	std::string validArgAdd;
	std::string validArgRem;
	std::string defaultKey = "*";
	bool action = REMOVE_MODE;
	std::string modeString = cmd.getTokens().at(2);
	for (char ch : modeString) {
		if (ch == '+' || ch == '-') {
			action = (ch == '+') ? ADD_MODE : REMOVE_MODE;
			continue;
		}
		else if (action == ADD_MODE) {
			switch (ch) {
				case 'i' :
					chan->addMode(Channel::ModeInviteOnly);
					validModeAdd += ch;
					break;
				case 't' :
					chan->addMode(Channel::ModeProtectedTopic);
					validModeAdd += ch;
					break;
				case 'k' :
					if (argID < cmd.getTokens().size() && this->updateChanKey(chan, cmd.getTokens().at(argID++))) {
						chan->addMode(Channel::ModeKeyOn);
						validModeAdd += ch;
						validArgAdd += cmd.getTokens().at(argID - 1) + " ";
					}
					break;
				case 'o' :
					if (argID < cmd.getTokens().size() && this->updateChanOper(c, chan, cmd.getTokens().at(argID++))) {
						chan->addMode(Channel::ModeOper);
						validModeAdd += ch;
						validArgAdd += cmd.getTokens().at(argID - 1) + " ";
					}
					break;
				case 'l' :
					if (argID < cmd.getTokens().size() && this->updateChanULimit(chan, cmd.getTokens().at(argID++))) {
						chan->addMode(Channel::ModeClientLim);
						validModeAdd += ch;
						validArgAdd += cmd.getTokens().at(argID - 1) + " ";
					}
					break;
				default:
					rpl = numericRPL(ERR_NOTONCHANNEL, nickName, target);//is this corrects message
					outBuf.append(rpl.c_str(), rpl.length());
			}
		}
		else if (action == REMOVE_MODE) {
			switch (ch) {
				case 'i' :
					chan->removeMode(Channel::ModeInviteOnly);
					validModeRem += ch;
					break;
				case 't' :
					chan->removeMode(Channel::ModeProtectedTopic);
					validModeRem += ch;
					break;
				case 'k' :
					if (argID++ < cmd.getTokens().size() && chan->getKeyMode() && this->updateChanKey(chan, defaultKey)) {
						chan->removeMode(Channel::ModeKeyOn);
						validModeRem += ch;
						validArgRem += defaultKey + " ";
					}
					break;
				case 'o' :
					if (argID < cmd.getTokens().size() && chan->chanOperatorRemove(cmd.getTokens().at(argID++))) {
						chan->removeMode(Channel::ModeOper);
						validModeRem += ch;
						validArgAdd += cmd.getTokens().at(argID - 1) + " ";
					}
					break;
				case 'l' :
					if (chan->getClientLimitMode()) {
						chan->removeMode(Channel::ModeClientLim);//do I have to set clientlim to 0? No I guess
						validModeRem += ch;
					}
					break;
				default:
					rpl = numericRPL(ERR_NOTONCHANNEL, nickName, target);//is this correct message
					outBuf.append(rpl.c_str(), rpl.length());
			}
		}
	}
	if(!validModeAdd.empty())
		validModeAdd = "+" + validModeAdd;
	if (!validModeRem.empty())
		validModeRem = "-" + validModeRem;
	std::string validModeArg = !(validArgAdd.empty() && validArgRem.empty())? validArgAdd + validArgRem : "";
	if (!(validModeAdd.empty() && validModeRem.empty()))
		rplRet = ":" + nickName + " MODE " + chan->getName() + " " + validModeAdd + validModeRem + " " + validModeArg + "\r\n"; //do I have to add # prefix to chanName? //add the parameters here as well
	return rplRet;//should rpl be reset to zero if no valid modes are applied. otherwie it will print twice //fixed the isue. should be tested for chan
}

std::string	Server::applyUserMode(Client& c, Command& cmd) {
	const std::string nickName = c.getNickName();
	Buffer &outBuf = c.getOutBuf();
	std::string rpl;
	std::string rplRet;
	std::string validModeAdd;
	std::string validModeRem;
	bool action = REMOVE_MODE;
	std::string modeString = cmd.getTokens().at(2);
	for (char ch : modeString) {
		if (ch == '+' || ch == '-') {
			action = (ch == '+') ? ADD_MODE : REMOVE_MODE;
			continue;
		}
		else if (action == ADD_MODE) {		
			switch (ch) {
				case 'i' :
					c.addMode(Client::ModeInvi);
					validModeAdd += ch;
					break;
				case 'o' :
					// c.addMode(Client::ModeOper);
					// validModeAdd += ch;
					break;//operator is added through OPER message not from this //should i neglect this silently or take action?
				default:
					rpl = numericRPL(ERR_UMODEUNKNOWNFLAG, nickName);
					outBuf.append(rpl.c_str(), rpl.length());
			}
		}
		else if (action == REMOVE_MODE) {
			switch (ch) {
				case 'i' :
					c.removeMode(Client::ModeInvi);
					validModeRem += ch;
					break;
				case 'o' :
					c.removeMode(Client::ModeOper);
					validModeRem += ch;
					break;
				default:
					rpl = numericRPL(ERR_UMODEUNKNOWNFLAG, nickName);
					outBuf.append(rpl.c_str(), rpl.length());
			}
		}
	}
	if(!validModeAdd.empty())
		validModeAdd = "+" + validModeAdd;
	if (!validModeRem.empty())
		validModeRem = "-" + validModeRem;
	if (!(validModeAdd.empty() && validModeRem.empty()))
		rplRet = ":" + nickName + " MODE " + validModeAdd + validModeRem + "\r\n"; //do I have to add # prefix to chanName? //add the parameters here as well
	return rplRet; //should rpl be reset to zero if no valid modes are applied. otherwie it will print twice //fixed the isue. should be tested for chan
}

//test cases: when chan limit is there but no key
//when key is there but no limit and also when user is a part of the channel and not a part of the channel
void	Server::mode(Client& c, Command& cmd)
{
	const std::string nickName = c.getNickName();
	Buffer& outBuf = c.getOutBuf();
	std::string rpl;
	if (cmd.getTokens().size() < 2) {
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens().at(0));
		outBuf.append(rpl.c_str(), rpl.length());
		return;
	}
	std::string target = cmd.getTokens().at(1);
	if (Channel::hasChanPrefix(target)) {
		auto it = Utils::getChannelIteratorByChannelName( _channels, target);
		if (it == _channels.end())
			rpl = numericRPL(ERR_NOSUCHCHANNEL, nickName, target);
		else if (cmd.getTokens().size() < 3) {
			std::string modeParam = "";
			if ((*it).hasMode(Channel::ModeKeyOn) && (*it).getChannelUsers().count(&c))
				modeParam = (*it).getKey();
			if ((*it).hasMode(Channel::ModeClientLim)) {
				if (!modeParam.empty())
					modeParam += " ";
				modeParam += std::to_string((*it).getClientLimit());
			}
			rpl = numericRPL(RPL_CHANNELMODEIS, nickName, target, (*it).getChanMode(), modeParam);
			rpl += numericRPL(RPL_CREATIONTIME, nickName, target, (*it).getTimeCreated());
		}
		else if (!(*it).isOperator(c))
			rpl = numericRPL(ERR_CHANOPRIVSNEEDED, nickName, target);
		else {
			if (this->isValidModeString(cmd.getTokens().at(2))) { //think about thie line and fix the isvalid modeString
				std::string  msgBroadcast= applyChanMode(c, &(*it), cmd);
				if (!msgBroadcast.empty())
					(*it).chanBroadcast(msgBroadcast);
			}	
		}
	} 
	else {
		if (!this->clientLookUp(target))
			rpl = numericRPL(ERR_NOSUCHNICK, nickName, target);
		else if (nickName != target)
			rpl = numericRPL(ERR_USERSDONTMATCH, nickName);
		else if (cmd.getTokens().size() < 3)
			rpl = numericRPL(RPL_UMODEIS, target, c.getUserMode());
		else
			rpl = applyUserMode(c, cmd);
	} 
	outBuf.append(rpl.c_str(), rpl.length());
}