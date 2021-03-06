/*
 * Dispatcher.h
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#ifndef SRC_DISPATCHER_H_
#define SRC_DISPATCHER_H_

#include "UsersManager.h"
#include "TCPSocket.h"
#include "MThread.h"
#include <vector>
#include "Models/User.h"

namespace npl {

class DispatcherHandler {
public:
	virtual void manageUsersGame(User* user1, User* user2)=0;
	virtual ~DispatcherHandler(){}
};

class Dispatcher : public MThread {
	/**
	 * listen to all peers and handle the incoming commands from the peer

	 */
	DispatcherHandler* handler;
	UsersManager* usersManager;

	bool isRunning;
private:
	static vector<TCPSocket*>* getSockets(vector<User*>* users );
	static vector<User*>* getAvailableUsers(vector<User*>* users );
	static User* getUserBySocket(vector<User*>* users, TCPSocket* sock);
	static User* getUserByUserName(vector<User*>* users, string userName);
	
	static void sendUser(TCPSocket* sock, User* user){
		TCPMessengerProtocol::sendData(sock, user->getUserName());
		TCPMessengerProtocol::sendData(sock, user->getIP());
		TCPMessengerProtocol::sendInt(sock, user->getUDPPort());
		
	}

	void handleUser(User* user);
	void disconnectUser(User* user);
	//game handling
	void requestToStartGame(User* user);
	void requestToStartRandomGame(User* user);
	void gameRequestRejected(User* requestedUser);
	void gameRequestAccepted(User* requestedUser);
	void startGameWithUser(User* sourceUser, User* targetUser);
	void gameEnded(User* user);

	//info handling
	void sendOnlineUsers(User* user);
	void sendScoreboard(User* user);
	
public:
	Dispatcher(UsersManager* usersManager, DispatcherHandler* handler);
	~Dispatcher(){}
	void userLoggedIn();
	void run();
	void close();



};

} /* namespace npl */

#endif /* SRC_DISPATCHER_H_ */
