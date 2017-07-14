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

Broker::Broker(TCPSocket* peer1, TCPSocket* peer2, BrokerMng* parent) {
	this->parent = parent;
	this->peer1 = peer1;
	this->peer2 = peer2;
	this->isRunning = false;
	this->start();
}

void Broker::run() {
	cout << "broker thread started" << endl;
	MTCPListener listener;
	listener.add(this->peer1);
	listener.add(this->peer2);
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

			int command;
			char data[1024];
			TCPMessengerProtocol::readCommand(command, data, sourcePeer);
			switch (command) {
			case SEND_MSG_TO_PEER:
				cout << "sending message: " << sourcePeer->fromAddr() << " -> "
						<< targetPeer->fromAddr() << endl;
				TCPMessengerProtocol::sendCommand(command, data, targetPeer);

				break;
			case CLOSE_SESSION_WITH_PEER:
			default:
				cout << "closing session: " << sourcePeer->fromAddr() << " -> "
						<< targetPeer->fromAddr() << endl;
				TCPMessengerProtocol::sendCommand(command, NULL, targetPeer);
				isRunning = false;
				break;
			}
		}
	}
	parent->releasePeer(peer1);
	parent->releasePeer(peer2);
	parent->deleteBroker(this);
	delete this;

}
void Broker::close() {
	isRunning = false;
	waitForThread();
}

} /* namespace npl */
