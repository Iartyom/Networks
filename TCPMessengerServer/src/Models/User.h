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
	bool busy;
	string connectedRoomName;
	User* connectedUser;
	int listeningPort;

public:
	User(string userName, TCPSocket* socket);
	User(string userName, string ip, int port, int listeningPort);
	virtual ~User();

	string getUserName();
	string getIP();
	int getPort();
	TCPSocket* getSocket();
	bool isBusy();
	void setBusy(User* user);
	void setBusy(string roomName);
	string getConnectedRoomName();
	User* getConnectedUser();
	void setAvailable();

	void setListeningPort(int port);
	int getListeningPort();
};

#endif /* MODELS_USER_H_ */
