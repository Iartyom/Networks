/*
 * Server.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#include "Server.h"
#include "TCPMessengerProtocol.h"

namespace npl {
Server::Server(ServerHandler* handler) {
	this->handler = handler;

	this->socket = new TCPSocket(MSNGR_PORT);
	if (this->socket->socket() >0) {
		this->isRunning = true;
		start();
	}
}

void Server::run() {
	this->isRunning = true;
	while (isRunning) {
		TCPSocket* peer = this->socket->listenAndAccept();
		if(peer!=NULL){
			handler->handlePeer(peer);
		}
	}
}

void Server::close() {
	this->isRunning = false;
	this->socket->close();

	waitForThread();

	delete this->socket;

}
} /* namespace npl */
