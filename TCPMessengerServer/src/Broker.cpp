/*
 * Broker.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#include "Broker.h"
#include "BrokerMng.h"
#include "TCPMessengerProtocol.h"

namespace npl {

Broker::Broker(User* user1, User* user2, BrokerMng* parent) {
	this->parent = parent;
	this->user1 = user1;
	this->user2 = user2;
	this->isRunning = false;
	this->start();
}

void Broker::run() {
	cout << "broker thread started" << endl;
	MTCPListener listener;
	listener.add(this->user1->getSocket());
	listener.add(this->user2->getSocket());
	isRunning = true;
	while (isRunning) {
		TCPSocket* sourcePeer = listener.listen(2);

		if (sourcePeer != NULL) {

			TCPSocket* targetPeer;
			if (sourcePeer == this->peer1) {
				targetPeer = this->peer2;
			} else {
				targetPeer = this->peer1;
			}

			int command = TCPMessengerProtocol::readCommand(userSocket);
			switch (command) {
			case GAME_ENDED:
				cout << "sending message: " << sourcePeer->fromAddr() << " -> "
						<< targetPeer->fromAddr() << endl;
				TCPMessengerProtocol::readData(command, data, targetPeer);

				break;
				isRunning = false;
			default:
				cout << "closing session: " << sourcePeer->fromAddr() << " -> "
						<< targetPeer->fromAddr() << endl;
				TCPMessengerProtocol::sendCommand(command, NULL, targetPeer);
				isRunning = false;
				break;
			}
		}
	}
	parent->gameEnded(user1, user2, result);
	parent->deleteBroker(this);
	delete this;

}
void Broker::close() {
	isRunning = false;
	waitForThread();
}

} /* namespace npl */
