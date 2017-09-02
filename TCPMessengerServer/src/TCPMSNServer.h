/*
 * TCPMSNServer.h
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#ifndef SRC_TCPMSNSERVER_H_
#define SRC_TCPMSNSERVER_H_

#include "Server.h"
#include "Dispatcher.h"
#include "UsersManager.h"
#include "UsersRepository.h"

namespace npl {

class TCPMSNServer: public ServerHandler,DispatcherHandler, LoginHandler {
	Server* server;
	Dispatcher* dispatcher;
	//BrokerMng* brokerMng;
	UsersManager* usersManager;
	UsersRepository* usersRepository;

public:
	TCPMSNServer();
	virtual void handlePeer(TCPSocket* peer);
	virtual void manageUsersGame(User* user1, User* user2);
	virtual void newUserLoggedIn(User* user);
	//virtual void handleReturnedPeer(TCPSocket* peer);

	void printAllUsers();
	void printConnectedUsers();
	void exit();

};

} /* namespace npl */

#endif /* SRC_TCPMSNSERVER_H_ */
