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
	usersManager = new UsersManager(usersRepository);
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

void TCPMSNServer::exit() {
	// this->brokerMng->close();
	// delete this->brokerMng;
	// cout<<"broker manager closed"<<endl;
	this->server->close();
	delete this->server;
	cout<<"server closed"<<endl;
	this->dispatcher->close();
	delete this->dispatcher;
	cout<<"dispatcher closed"<<endl;


}

} /* namespace npl */
