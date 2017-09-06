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
	vector<TCPSocket*>* sockets = new vector<TCPSocket*>();
	vector<User*>::iterator it = users->begin();
	for(; it != users->end(); ++it){
		User* user = (*it);
		sockets->push_back(user->getSocket());
	}
	return sockets;
}

vector<User*>* Dispatcher::getAvailableUsers(vector<User*>* users ){
	vector<User*>* availableUsers = new vector<User*>();
	vector<User*>::iterator it = users->begin();
	for(; it != users->end(); ++it){
		User* user = (*it);
		if(!user->isBusy()){
			availableUsers->push_back(user);
		}
	}
	return availableUsers;
}

User* Dispatcher::getUserBySocket(vector<User*>* users, TCPSocket* sock){
	
	vector<User*>::iterator it = users->begin();
	for(; it != users->end(); ++it){
		User* user = (*it);
		if(user->getSocket() == sock){
			return user;
		}
	}
	return NULL;
}
User* Dispatcher::getUserByUserName(vector<User*>* users, string userName){
	
	vector<User*>::iterator it = users->begin();
	for(; it != users->end(); ++it){
		User* user = (*it);
		if(user->getUserName().compare(userName) == 0){
			
			return user;
		}
	}
	return NULL;
}

Dispatcher::Dispatcher(UsersManager* usersManager, DispatcherHandler* handler) {
	this->handler = handler;
	this->usersManager = usersManager;
	this->isRunning = false;
	srand(time(NULL));
}

void Dispatcher::userLoggedIn() {
	if (!isRunning) {
		isRunning = true;
		start();
	}
}

void Dispatcher::run() {
	cout << "dispatcher started" << endl;
	while (isRunning) {
		vector<User*>* loggedUsers = this->usersManager->getLoggedInUsers();
		if (loggedUsers->size() == 0) {
			isRunning = false;
			break;
		}
		MTCPListener listener;
		
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
		case REQUEST_TO_START_RANDOM_GAME:
			this->requestToStartRandomGame(user);
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
			this->sendScoreboard(user);
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
	if(targetUserName == user->getUserName()){
		TCPMessengerProtocol::sendCommand(user->getSocket(), SESSION_REFUSED);
		return;
	}
	User* targetUser = Dispatcher::getUserByUserName(usersManager->getLoggedInUsers(),targetUserName);
	if(targetUser == NULL || targetUser->isBusy()){
		cout << targetUserName << " - no available user found" << endl;
		TCPMessengerProtocol::sendCommand(user->getSocket(), SESSION_REFUSED);
		return;
	}
	cout <<"request to start new game: " << user->getUserName() << "-> " << targetUser->getUserName() << endl;
	TCPMessengerProtocol::sendCommand(targetUser->getSocket(), REQUEST_TO_START_GAME);
	TCPMessengerProtocol::sendData(targetUser->getSocket(), user->getUserName());
}
void Dispatcher::requestToStartRandomGame(User* user){
	vector<User*>* availableUsers = Dispatcher::getAvailableUsers(usersManager->getLoggedInUsers());
	if(availableUsers->size()>1){
		User* randomUser;
		do{
			randomUser = availableUsers->at(rand() % (availableUsers->size()));
		} while(user->getUserName().compare(randomUser->getUserName())==0);

		cout <<"request to start random new game: " << user->getUserName() << " -> " << randomUser->getUserName() << endl;
		TCPMessengerProtocol::sendCommand(randomUser->getSocket(), REQUEST_TO_START_GAME);
		TCPMessengerProtocol::sendData(randomUser->getSocket(), user->getUserName());
	} else {
		cout <<"no online users for request random new game from: " << user->getUserName() << endl;
		TCPMessengerProtocol::sendCommand(user->getSocket(), SESSION_REFUSED);
	}
}
	
void Dispatcher::gameRequestRejected(User* requestedUser){
	string requestingUserName = TCPMessengerProtocol::readData(requestedUser->getSocket());
	User* requestingUser = Dispatcher::getUserByUserName(usersManager->getLoggedInUsers(),requestingUserName);
	cout << requestedUser->getUserName() << " rejected game request from " << requestingUser->getUserName() << endl;
	
	TCPMessengerProtocol::sendCommand(requestingUser->getSocket(), GAME_REQUEST_REJECTED);
}

void Dispatcher::gameRequestAccepted(User* requestedUser){
	string requestingtargetUserName = TCPMessengerProtocol::readData(requestedUser->getSocket());
	User* requestingUser = Dispatcher::getUserByUserName(usersManager->getLoggedInUsers(),requestingtargetUserName);
	cout << requestedUser->getUserName() << " accepted game request from " << requestingUser->getUserName() << endl;
	
	startGameWithUser(requestingUser, requestedUser);
}

void Dispatcher::startGameWithUser(User* sourceUser, User* targetUser){
	cout <<"starting game between : " << sourceUser->getUserName() << "-> " << targetUser->getUserName() << endl;
	
	int sourceUDPPort = rand() % 1000 + MSNGR_CLIENT_PORT;
	int targetUDPPort;
	do{
		targetUDPPort = rand() % 1000 + MSNGR_CLIENT_PORT;
	}while(sourceUDPPort == targetUDPPort);

	sourceUser->setUDPPort(sourceUDPPort);
	targetUser->setUDPPort(targetUDPPort);
	
	TCPMessengerProtocol::sendCommand(sourceUser->getSocket(), START_GAME);
	TCPMessengerProtocol::sendInt(sourceUser->getSocket(), sourceUDPPort);
	Dispatcher::sendUser(sourceUser->getSocket(), targetUser);

	TCPMessengerProtocol::sendCommand(targetUser->getSocket(), START_GAME);
	TCPMessengerProtocol::sendInt(targetUser->getSocket(), targetUDPPort);
	Dispatcher::sendUser(targetUser->getSocket(), sourceUser);

	sourceUser->setBusy(targetUser);
	targetUser->setBusy(sourceUser);
}

void Dispatcher::gameEnded(User* user){
	int gameResult = TCPMessengerProtocol::readInt(user->getSocket());
	int score;
	

	switch(gameResult){
		case WIN:
			//updating users score
			score = TCPMessengerProtocol::readInt(user->getSocket());
			this->usersManager->addScore(user, score);
			cout <<"game ended: " << user->getUserName() << " win -> " << user->getConnectedUser()->getUserName() << endl;
			
			break;
		case LOSE:
			score = TCPMessengerProtocol::readInt(user->getSocket());
			this->usersManager->addScore(user, score);
			cout <<"game ended: " << user->getUserName() << " lost -> " << user->getConnectedUser()->getUserName() << endl;
		
			break;
		
		case DRAW:
			score = TCPMessengerProtocol::readInt(user->getSocket());
			this->usersManager->addScore(user, score);
			cout <<"game ended draw: " << user->getUserName() << " -> " << user->getConnectedUser() << endl;
			
			break;
		case CANCELLED:
		default:
			cout <<"game cancelled: " << user->getUserName() << " -> " << user->getConnectedUser() << endl;
		
		
	}

	user->setAvailable();
	
}

//info handling

void Dispatcher::sendOnlineUsers(User* user){
	TCPSocket* userSocket = user->getSocket();
	vector<User*>* users = this->usersManager->getLoggedInUsers();
	
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
}

void Dispatcher::sendScoreboard(User* user){
	TCPSocket* userSocket = user->getSocket();
	TCPMessengerProtocol::sendCommand(userSocket, GET_SCOREBOARD);

	vector<std::pair<string,int>> scoreboardVector = this->usersManager->getScoreboard();
	TCPMessengerProtocol::sendInt(userSocket, scoreboardVector.size()); //users length
	
	vector<std::pair<string,int>>::iterator it = scoreboardVector.begin();
	for(;it != scoreboardVector.end(); ++it){
		TCPMessengerProtocol::sendData(userSocket, (*it).first); //user name
		TCPMessengerProtocol::sendInt(userSocket, (*it).second); //score
	}

}

void Dispatcher::disconnectUser(User* user){
	
	if(user->isBusy()){
		if(user->getConnectedUser()!=NULL){
			TCPMessengerProtocol::sendCommand(user->getConnectedUser()->getSocket(), GAME_ENDED);
			
			user->getConnectedUser()->setAvailable();
			
		}
		user->setAvailable();
	}

	this->usersManager->logout(user);
}

void Dispatcher::close() {
	this->isRunning = false;
	this->waitForThread();
}


} /* namespace npl */
