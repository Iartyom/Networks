/*
 * LoginManager.h
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#ifndef LOGINMANAGER_H_
#define LOGINMANAGER_H_

#include <string.h>
#include <map>
#include <vector>
#include<iostream>
#include<fstream>
#include "MThread.h"
#include "TCPSocket.h"
#include "MTCPListener.h"
#include "UsersRepository.h"
#include "TCPMessengerProtocol.h"
#include "Models/User.h"

using namespace std;
namespace npl{
class LoginHandler {
public:
	virtual void newUserLoggedIn(User* user)=0;
	virtual ~LoginHandler(){}
};
class LoginManager : public MThread{
	vector<TCPSocket*>* notLoggedInPeers;
	UsersRepository* usersRepository;
	bool isRunning;
private:
	void addToLoggedIn(string userName, TCPSocket* peer);
	void removePeer(TCPSocket* peer);
public:
	LoginManager(LoginHandler* handler, UsersRepository* usersRepository);
	virtual ~LoginManager();

	bool login(string userName,string password);
	bool registerUser(string userName, string password);
	void logout(User* user);

	vector<string> getAllUsers();
	bool isUserLoggedIn(string userName);
	vector<User*>* getLoggedInUsers();

	void addPeer(TCPSocket* peer);
	void run();
	void close();
};
}

#endif /* LOGINMANAGER_H_ */
