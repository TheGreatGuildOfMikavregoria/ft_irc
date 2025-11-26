
			/*
			char *resp = (char *)"PASS RESPONSE\n\r";
			if (message.getCommand() == "NICK")
			{
				if (message.getTokens().size() == 1)
				{
					char *err_msg = (char *)"NICK WRONG TOKEN NUM\n\r";
					c.getOutBuf().append(err_msg, strlen(err_msg));
				}
				else if (message.getTokens().size() == 2)
				{
					c.setNickName(message.getTokens()[1]);
					c.setNickNameStatus(true);
					if (!c.getAuthStatus() && c.getUserNameStatus())
					{
						c.setAuthStatus(true);
						c.getOutBuf().append((char *)"001", strlen("001"));
						c.getOutBuf().append((char *)" ", strlen(" "));
						c.getOutBuf().append((char *)c.getNickName().c_str(), c.getNickName().length());
						c.getOutBuf().append((char *)" ", strlen(" "));
						c.getOutBuf().append((char *)":Welcome to the networkname Network\n\r", strlen(":Welcome to the networkname Network\n\r"));
					}
				}
			}
			if (message.getCommand() == "USER")
			{
				if (message.getTokens().size() < 5)
				{
					char *err_msg = (char *)"USER WRONG TOKEN NUM\n\r";
					c.getOutBuf().append(err_msg, strlen(err_msg));
				}
				else if (message.getTokens().size() == 5)
				{
					c.setUserName(message.getTokens()[1]);
					c.setUserNameStatus(true);
					if (!c.getAuthStatus() && c.getNickNameStatus())
					{
						c.setAuthStatus(true);
						c.getOutBuf().append((char *)"001", strlen("001"));
						c.getOutBuf().append((char *)" ", strlen(" "));
						c.getOutBuf().append((char *)c.getNickName().c_str(), c.getNickName().length());
						c.getOutBuf().append((char *)" ", strlen(" "));
						c.getOutBuf().append((char *)":Welcome to the <networkname> Network\n\r", strlen(":Welcome to the <networkname> Network\n\r"));
					}
				}
			}

			if (message.getCommand() == "PING")
			{
				if (message.getTokens().size() < 2)
				{
					char *err_msg = (char *)"PING WRONG TOKEN NUM\n\r";
					c.getOutBuf().append(err_msg, strlen(err_msg));
				}
				else
				{
//					c.setAuthStatus(true);
					c.getOutBuf().append((char *)"PONG", strlen("PONG"));
					c.getOutBuf().append((char *)" ", strlen(" "));
					c.getOutBuf().append((char *)":", strlen(":"));
					c.getOutBuf().append((char *)message.getTokens()[1].c_str(), message.getTokens()[1].length());
					c.getOutBuf().append((char *)"\n\r", strlen("\n\r"));
				}
			}

			*/
