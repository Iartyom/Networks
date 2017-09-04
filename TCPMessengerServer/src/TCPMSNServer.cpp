/*
 * TCPMSNServer.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#include "TCPMSNServer.h"
#include "Server.h"
#include "Dispatcher.h"
#include <iostream>
using namespace std;

namespace npl {

TCPMSNServer::TCPMSNServer() {
	usersRepository = new UsersRepository();
	usersManager = new UsersManager(usersRepository, this);
	server = new Server(this);
	dispatcher = new Dispatcher(usersManager, this);
	//brokerMng = new BrokerMng(this);
}

void TCPMSNServer::handlePeer(TCPSocket* peer) {
	cout<<"new peer connected - "<<peer->fromAddr()<<endl;
	this->usersManager->addPeer(peer);
}
void TCPMSNServer::manageUsersGame(User* user1, User* user2) {
	//this->brokerMng->createBroker(user1, user2);
}
// void TCPMSNServer::handleReturnedPeer(TCPSocket* peer) {
// 	this->dispatcher->addPeer(peer);
// }
void TCPMSNServer::newUserLoggedIn(){
	this->dispatcher->userLoggedIn();
}
void TCPMSNServer::printAllUsers(){
	cout << "All Users: " << endl;

	vector<string> users = this->usersRepository->getAllUsers();

	vector<string>::iterator it = users.begin();
	for(;it != users.end(); ++it){
		cout << *it << endl;
	}

}
void TCPMSNServer::printConnectedUsers(){
	cout << "Connected Users: " << endl;

	vector<User*>* users = this->usersManager->getLoggedInUsers();
	//sem->lock();
	vector<User*>::iterator it = users->begin();
	for(;it != users->end(); ++it){
		cout << (*it)->getUserName() << endl;
	}
	//sem->unlock();
}
void TCPMSNServer::printScoreboard(){
	int counter = 0;
	vector<std::pair<string,int>> scoreboardVector = this->usersRepository->getOrderedScoreboard();

	cout << "Scoreboard:" << endl;
	vector<std::pair<string,int>>::iterator it = scoreboardVector.begin();
	for(;it != scoreboardVector.end(); ++it){

		cout <<++counter<< "Name: " <<(*it).first <<", Score: "<< (*it).second<< endl;
	}
	//sem->unlock();
}
void TCPMSNServer::exit() {
	this->server->close();
	delete this->server;
	cout<<"server closed"<<endl;

	if(this->dispatcher != NULL){
		this->dispatcher->close();
		delete this->dispatcher;
		this->dispatcher = NULL;
	}

	if(this->usersManager != NULL){
		this->usersManager->close();
		delete this->usersManager;
		this->usersManager = NULL;
	}


}

} /* namespace npl */
