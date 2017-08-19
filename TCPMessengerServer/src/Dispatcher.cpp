/*
 * Dispatcher.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#include "Dispatcher.h"
#include "MTCPListener.h"
#include "TCPMessengerProtocol.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

namespace npl {



vector<TCPSocket*>* Dispatcher::getSockets(vector<User*>* users ){
	vector<TCPSocket*> sockets = new vector<TCPSocket*>();
	vector<User*>::iterator it = users->begin();
	for(; it != users->end(); ++it){
		User* user = (*it);
		sockets.push_back(user->getSocket());
	}
	return sockets;
}

User* Dispatcher::getUserBySocket(vector<User*>* users, TCPSocket* sock){
	
	//sem->lock();
	vector<User*>::iterator it = users->begin();
	for(; it != users->end(); ++it){
		User* user = (*it);
		if(user->getSocket() == sock){
			//sem->unlock();
			return user;
		}
	}
	//sem->unlock();
	return NULL;
}
User* Dispatcher::getUserByUserName(vector<User*>* users, string userName){
	//sem->lock();
	vector<User*>::iterator it = users->begin();
	for(; it != users->end(); ++it){
		User* user = (*it);
		if(user->getUserName().compare(userName) == 0){
			//sem->unlock();
			return user;
		}
	}
	//sem->unlock();
	return NULL;
}

Dispatcher::Dispatcher(UsersManager* usersManager, DispatcherHandler* handler) {
	this->handler = handler;
	this->usersManager = usersManager;
	this->isRunning = false;
}
// void Dispatcher::addPeer(TCPSocket* peer) {
// 	peers.push_back(peer);
// 	if (!isRunning) {
// 		isRunning = true;
// 		start();
// 	}
// }

void Dispatcher::run() {
	cout << "dispatcher started" << endl;
	while (isRunning) {
		// if (peers.size() == 0) {
		// 	isRunning = false;
		// 	break;
		// }
		MTCPListener listener;
		vector<User*>* loggedUsers = this->usersManager->getLoggedInUsers();
		vector<TCPSocket*>* userSockets = Dispatcher::getSockets(loggedUsers);
		listener.add(*userSockets);
		TCPSocket* userSocket = listener.listen(5);
		delete userSockets;

		if (userSocket != NULL) {
			User* user = Dispatcher::getUserBySocket(loggedUsers, userSocket);			
			handleUser(user);
		}

	}
	cout << "dispatcher ended" << endl;

}

void Dispatcher::handleUser(User* user){
	TCPSocket* userSocket = user->getSocket();
	int command = TCPMessengerProtocol::readCommand(userSocket);

	switch(command){
		case REQUEST_TO_START_GAME:
			this->requestToStartGame(user);
			break;
		case GAME_REQUEST_REJECTED:
			this->gameRequestRejected(user);
			break;
		case GAME_REQUEST_ACCEPTED:
			this->gameRequestAccepted(user);
			break;
		case GAME_ENDED:
			this->gameEnded(user);
			break;
		case GET_ONLINE_USERS_LIST:
			this->sendOnlineUsers(user);
			break;
		case GET_SCOREBOARD:
			
			break;
		case EXIT:
			this->disconnectUser(user);
			break;
		default:
			cout << command << " unexpected command, closing connection with: " << user->getUserName() << endl;
			this->disconnectUser(user);
			break;

	}
}


//game handling
void Dispatcher::requestToStartGame(User* user){
	string targetUserName = TCPMessengerProtocol::readData(user->getSocket());
	User* targetUser = Dispatcher::getUserByUserName(usersManager->loggedInUsers,targetUserName);
	if(targetUser == NULL || targetUser->isBusy()){
		cout << targetUserName << " - no available user found" << endl;
		TCPMessengerProtocol::sendCommand(user->getSocket(), SESSION_REFUSED);
		return;
	}
	cout <<"request to start new game: " << user->getUserName() << "-> " << targetUser->getUserName() << endl;
	TCPMessengerProtocol::sendCommand(targetUser->getSocket(), REQUEST_TO_START_GAME);
	TCPMessengerProtocol::sendData(targetUser->getSocket(), user->getUserName());
}

void Dispatcher::gameRequestRejected(User* requestedUser){
	string requestingUserName = TCPMessengerProtocol::readData(requestedUser->getSocket());
	User* requestingUser = Dispatcher::getUserByUserName(usersManager->loggedInUsers,requestingUserName);
	cout << requestedUser->getUserName() << " rejected game request from " << requestingUser->getUserName() << endl;
	
	TCPMessengerProtocol::sendCommand(requestingUser->getSocket(), GAME_REQUEST_REJECTED);
}

void Dispatcher::gameRequestAccepted(User* requestedUser){
	string requestingtargetUserName = TCPMessengerProtocol::readData(requestedUser->getSocket());
	User* requestingUser = Dispatcher::getUserByUserName(usersManager->loggedInUsers,requestingtargetUserName);
	cout << requestedUser->getUserName() << " accepted game request from " << requestingUser->getUserName() << endl;
	
	startGameWithUser(requestingUser, requestedUser);
}

void Dispatcher::startGameWithUser(User* sourceUser, User* targetUser){
	cout <<"starting game between : " << sourceUser->getUserName() << "-> " << targetUser->getUserName() << endl;
	srand(time(NULL));
	int sourcePort = rand() % 1000 + MSNGR_CLIENT_PORT;
	int targetPort;
	do{
		targetPort = rand() % 1000 + MSNGR_CLIENT_PORT;
	}while(sourcePort == targetPort);

	sourceUser->setListeningPort(sourcePort);
	targetUser->setListeningPort(targetPort);

	TCPMessengerProtocol::sendCommand(targetUser->getSocket(), START_GAME);
	TCPMessengerProtocol::sendInt(targetUser->getSocket(), targetPort); // send the port should listen
	Dispatcher::sendUser(targetUser->getSocket(), sourceUser);

	TCPMessengerProtocol::sendCommand(sourceUser->getSocket(), START_GAME);
	TCPMessengerProtocol::sendInt(sourceUser->getSocket(), sourcePort); // send the port should listen
	Dispatcher::sendUser(sourceUser->getSocket(), targetUser);

	sourceUser->setBusy(targetUser);
	targetUser->setBusy(sourceUser);
	//this->addSession(sourceUser->getUserName(), targetUser->getUserName());
}

void Dispatcher::gameEnded(User* user){
	string opponentUserName = TCPMessengerProtocol::readData(user->getSocket());
	User* opponentUser = Dispatcher::getUserByUserName(usersManager->loggedInUsers, opponentUserName);
	int gameResult = TCPMessengerProtocol::readInt(user->getSocket());

	switch(gameResult){
		case WINLOSE:
			//updating users score
			string winnerUserName = TCPMessengerProtocol::readData(user->getSocket());
			int winnerScore = TCPMessengerProtocol::readInt(user->getSocket());
			string loserUserName = TCPMessengerProtocol::readData(user->getSocket());
			int loserScore = TCPMessengerProtocol::readInt(user->getSocket());
			
			this->usersManager->addScore(Dispatcher::getUserByUserName(usersManager->loggedInUsers,winnerUserName), winnerScore);
			this->usersManager->addScore(Dispatcher::getUserByUserName(usersManager->loggedInUsers,loserUserName), loserScore);
			
			TCPMessengerProtocol::sendCommand(opponentUser->getSocket(), GAME_ENDED);
			TCPMessengerProtocol::sendData(opponentUser->getSocket(), user->getUserName());
			TCPMessengerProtocol::sendInt(opponentUser->getSocket(), gameResult);
			TCPMessengerProtocol::sendData(opponentUser->getSocket(), winnerUserName);
			

			break;
		case DRAW:
			int drawScore = TCPMessengerProtocol::readInt(user->getSocket());
			this->usersManager->addScore(user, drawScore);
			this->usersManager->addScore(opponentUser, drawScore);

			TCPMessengerProtocol::sendCommand(opponentUser->getSocket(), GAME_ENDED);
			TCPMessengerProtocol::sendData(opponentUser->getSocket(), user->getUserName());
			TCPMessengerProtocol::sendInt(opponentUser->getSocket(), gameResult);
			
			break;
		case CANCELLED:
		default:
			TCPMessengerProtocol::sendCommand(opponentUser->getSocket(), GAME_ENDED);
			TCPMessengerProtocol::sendData(opponentUser->getSocket(), user->getUserName());
			TCPMessengerProtocol::sendInt(opponentUser->getSocket(), gameResult);
			
	}
	
}

//info handling

void Dispatcher::sendOnlineUsers(User* user){
	TCPSocket* userSocket = user->getSocket();
	vector<User*>* users = this->usersManager->getLoggedInUsers();
	//sem->lock();
	TCPMessengerProtocol::sendCommand(userSocket, GET_ONLINE_USERS_LIST);
	TCPMessengerProtocol::sendInt(userSocket, users->size());
	vector<User*>::iterator it = users->begin();
	for(; it != users->end(); ++it){
		string userStatus;
		if((*it)->isBusy()){
			userStatus="busy";
		} else{
			userStatus="free";
		}
		TCPMessengerProtocol::sendData(userSocket, (*it)->getUserName());
		TCPMessengerProtocol::sendData(userSocket, userStatus);
	}
	//sem->unlock();
}



// void Dispatcher::listPeers() {
// 	vector<TCPSocket*>::iterator iter = peers.begin();
// 	for (; iter != peers.end(); iter++) {
// 		TCPSocket* peer = *iter;
// 		cout << peer->fromAddr() << endl;
// 	}
// }
void Dispatcher::close() {
	// this->isRunning = false;
	// this->waitForThread();

	// vector<TCPSocket*>::iterator iter = peers.begin();
	// for (; iter != peers.end(); iter++) {
	// 	TCPSocket* peer = *iter;
	// 	peer->close();
	// 	delete peer;
	// }
	// this->peers.clear();
}

// TCPSocket* Dispatcher::findAvailablePeer(const char* peerAddr) {
// 	vector<TCPSocket*>::iterator iter = peers.begin();
// 	for (; iter != peers.end(); iter++) {
// 		TCPSocket* sock = *iter;
// 		if (strcmp(sock->fromAddr().c_str(), peerAddr) == 0) {
// 			return sock;
// 		}
// 	}
// 	return NULL;

// }

} /* namespace npl */
