/*
 * User.h
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#ifndef MODELS_USER_H_
#define MODELS_USER_H_

#include <string.h>
#include "TCPSocket.h"

using namespace std;
using namespace npl;
class User {
	string userName;
	TCPSocket* socket;
	string initialIP;
	bool busy;
	int port;
	User* connectedUser;

public:
	User(string userName, TCPSocket* socket);
	virtual ~User();

	string getUserName();
	string getIP();
	int getUDPPort();
	TCPSocket* getSocket();
	User* getConnectedUser();

	bool isBusy();
	void setUDPPort();
	void setBusy(User* user);
	void setAvailable();
};

#endif /* MODELS_USER_H_ */
