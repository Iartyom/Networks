/*
 * UsersManager.h
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#ifndef UsersManager_H_
#define UsersManager_H_

#include <string.h>
#include <map>
#include <vector>
#include<iostream>
#include <pthread.h>
#include "Guard.h"
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
	virtual void newUserLoggedIn()=0;
	virtual ~LoginHandler(){}
};
class UsersManager : public MThread{
	LoginHandler* loginHandler;
	vector<TCPSocket*>* notLoggedInPeers;
	vector<User*>* loggedInUsers;
	UsersRepository* usersRepository;

	bool isRunning;
	pthread_mutex_t mutex;
private:
	void addToLoggedIn(string userName, TCPSocket* peer);
	void removePeer(TCPSocket* peer);
	void addPeer(TCPSocket* peer);
public:
	UsersManager(UsersRepository* usersRepository, LoginHandler* handler);
	virtual ~UsersManager();
	
	bool login(string userName,string password);
	bool registerUser(string userName, string password);
	void logout(User* user);

	bool isUserLoggedIn(string userName);
	vector<User*>* getLoggedInUsers();

	void addScore(User* user, int score);

	void run();
	void close();
};
}

#endif /* UsersManager_H_ */
