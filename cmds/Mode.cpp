#include "../Server.hpp"
#define VALID_USER_MODES "iows"
#define VALID_CHAN_MODES "itkol"
#define MODE_CHAN 1
#define MODE_USER 0
#define ADD_MODE 1
#define REMOVE_MODE 0

bool	isValidModeString(const std::string& modeString, bool whichMode){
	if (modeString.size() < 2 || !(modeString[0] == '+' || modeString[0] == '-'))
		return false;
	// int  signCount = 1;
	std::string validModeSet = whichMode ? VALID_CHAN_MODES : VALID_USER_MODES;
		for (size_t i = 1; i < modeString.size(); ++i) {
			char ch = modeString[i];
			if (ch == '+' || ch == '-') {
				// if (++signCount > 1 || i == modeString.size() - 1)
				// 	return false;
				continue;
			}
			// signCount = 0;
			if (validModeSet.find(ch) == std::string::npos) {
				return false;
		}
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

//if mode arguments are not enough still the valid modes should be processes.
//liberschat writes channels modes fore MODE command even if the client is not a member. but shouldn't let client change the modes.
std::string	Server::applyChanMode(Client& c, Channel* chan, Command& cmd) {
	const std::string nickName = c.getNickName();
	std::string target = cmd.getTokens().at(1);
	int argID = cmd.getTokens().size() >= 4 ? 3 : 0;
	std::string rpl;
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
					break;
				case 't' :
					chan->addMode(Channel::ModeProtectedTopic);
					break;
				case 'k' :
					if (argID < cmd.getTokens().size() && this->updateChanKey(chan, cmd.getTokens().at(argID)))
						chan->addMode(Channel::ModeKeyOn);
					break;
				case 'o' :

					chan->addMode(Channel::ModeKeyOn);
					break;
				case 'l' :
					chan->addMode(Channel::ModeKeyOn);
					break;
				
			}
		}
		else if (action == REMOVE_MODE) {
			switch (ch) {
				case 'i' :
					chan->removeMode(Channel::ModeInviteOnly);
					break;
				case 't' :
					chan->removeMode(Channel::ModeProtectedTopic);
					break;
				case 'k' :
					chan->removeMode(Channel::ModeKeyOn);
					break;
				case 'o' :
					chan->removeMode(Channel::ModeKeyOn);
					break;
				case 'l' :
					chan->removeMode(Channel::ModeKeyOn);
					break;
			}
		}
	}
}

void	Server::mode(Client& c, Command& cmd)
{
	const std::string nickName = c.getNickName();
	std::string target = cmd.getTokens().at(1);
	Buffer& outBuf = c.getOutBuf();
	std::string rpl;
	if (cmd.getTokens().size() < 2)
		rpl = numericRPL(ERR_NEEDMOREPARAMS, nickName, cmd.getTokens().at(0));
	else if (Channel::hasChanPrefix(target)) {
		auto it = Utils::getChannelIteratorByChannelName( _channels, target);
		if (it == _channels.end())
			rpl = numericRPL(ERR_NOSUCHCHANNEL, nickName, target);
		// else if (!(*it).getChannelUsers().count(&c))
		// 	rpl = numericRPL(ERR_NOTONCHANNEL, nickName, target);
		else if (!(*it).isOperator(c))
			rpl = numericRPL(ERR_CHANOPRIVSNEEDED, nickName, target);
		else if (cmd.getTokens().size() < 3) {
			rpl = numericRPL(RPL_CHANNELMODEIS, nickName, target, (*it).getChanMode(), (*it).getClientLimit());
			rpl += numericRPL(RPL_CREATIONTIME, nickName, target, (*it).getTimeCreated());
		}
		else {
			if (this->isValidModeString(cmd.getTokens().at(2), MODE_CHAN))
				rpl = applyChanMode(c, &(*it), cmd);
		}
	}
}