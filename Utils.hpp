#pragma once
#include <string>
#include <algorithm>
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

std::string stringToLowercase(std::string text);

auto getUserIteratorByNickName(std::vector<Client> &clientVector, const std::string &nickName) -> std::vector<Client>::iterator;

auto getChannelIteratorByChannelName(std::vector<Channel> &clientVector, const std::string &name) -> std::vector<Channel>::iterator;
auto getStringIteratorByString(std::vector<std::string> &stringVector, const std::string &str) -> std::vector<std::string>::iterator;
