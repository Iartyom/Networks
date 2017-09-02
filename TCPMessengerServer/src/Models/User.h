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
	User* connectedUser;

public:
	User(string userName, TCPSocket* socket);
	User(string userName, string ip);
	virtual ~User();

	string getUserName();
	string getIP();
	TCPSocket* getSocket();
	User* getConnectedUser();

	bool isBusy();
	
	void setBusy(User* user);
	void setAvailable();
};

#endif /* MODELS_USER_H_ */
