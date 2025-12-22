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
	int  signCount = 1;
	std::string validModeSet = whichMode ? VALID_CHAN_MODES : VALID_USER_MODES;
		for (size_t i = 1; i < modeString.size(); ++i) {
			char c = modeString[i];
			if (c == '+' || c == '-') {
				if (++signCount > 1 || i == modeString.size() - 1)
					return false;
				continue;
			}
			signCount = 0;
			if (validModeSet.find(c) == std::string::npos) {
				return false;
		}
	}
	return true;
}

std::string	Server::applyChanMode(Client& c, Channel* chan, Command& cmd) {
	const std::string nickName = c.getNickName();
	std::string target = cmd.getTokens().at(1);
	int argID = cmd.getTokens().size() >= 4 ? 3 : 0;
	std::string rpl;
	bool action = REMOVE_MODE;
	std::string modeString = cmd.getTokens().at(2);
	for (char c : modeString) {
		if (c == '+')
			action = ADD_MODE;
		else if (c == '-')
			action = REMOVE_MODE;
		else {
			switch (action) {
				case ADD_MODE :
					switch (c) {
						case 'i' :
							chan->addMode(Channel::ModeInviteOnly);
							break;
						case 't' :
							chan->addMode(Channel::ModeProtectedTopic);
							break;
						case 'k' :


					}
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
		else if (cmd.getTokens().size() < 3) {
			rpl = numericRPL(RPL_CHANNELMODEIS, nickName, target, (*it).getChanMode(), (*it).getClientLimit());
			rpl += numericRPL(RPL_CREATIONTIME, nickName, target, (*it).getTimeCreated());
		}
		else {
			if (isValidModeString(cmd.getTokens().at(2), MODE_CHAN))
				rpl = applyChanMode(c, &(*it), cmd);
		}
	}
}