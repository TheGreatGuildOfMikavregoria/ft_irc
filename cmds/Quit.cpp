#include "../Server.hpp"

void Server::quit(Client& c, Command& cmd) {
	const std::string nickName = c.getNickName();
	std::string reason;
	if (!(cmd.getTokens().size() < 2))
		reason = cmd.getTokens().at(1);
	// Buffer& outBuf = c.getOutBuf();
 	std::string rpl;
 	rpl = "Closing Link: " + nickName +  " (Quit: " + reason + "!)";
 	this -> error(c,rpl);
 	rpl = ":" + nickName + " QUIT :" +  reason + "\r\n";
 	std::set<Channel*>::iterator it;
 	for (it = c.getUserChannels().begin(); it != c.getUserChannels().end();) {
		Channel* chan = *it;
 		it++;
		chan->userRemove(c);
		chan->chanBroadcast(c, rpl);
 	}
	c.getUserChannels().clear();
	c.setDisconnectFlag(true);
}

void Server::error(Client& c, const std::string& msg) {
	std::string err_msg;
	Buffer& outBuf = c.getOutBuf();
	err_msg = "ERROR :" + msg + "\r\n";
	outBuf.append(err_msg.c_str(), err_msg.length());
}