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
Dispatcher::Dispatcher(DispatcherHandler* handler) {
	this->handler = handler;
	this->isRunning = false;
}
void Dispatcher::addPeer(TCPSocket* peer) {
	peers.push_back(peer);
	if (!isRunning) {
		isRunning = true;
		start();
	}
}
;
void Dispatcher::run() {
	cout << "dispatcher started" << endl;
	while (isRunning) {
		if (peers.size() == 0) {
			isRunning = false;
			break;
		}
		MTCPListener listener;
		listener.add(peers);
		TCPSocket* peer = listener.listen(5);
		if (peer != NULL) {
			int command;
			char data[1024];
			TCPMessengerProtocol::readCommand(command, data, peer);

			switch (command) {
			case OPEN_SESSION_WITH_PEER: {
				cout << "got open session command:" << peer->fromAddr()
						<< " -> " << data << endl;
				TCPSocket* targetPeer = findAvailablePeer(data);
				if (targetPeer == NULL) {
					cout << "FAIL: didn't find peer " << data << endl;
					TCPMessengerProtocol::sendCommand(SESSION_REFUSED, NULL,
							peer);
					break;
				}
				TCPMessengerProtocol::sendCommand(OPEN_SESSION_WITH_PEER,
						targetPeer->fromAddr().c_str(), targetPeer);
				TCPMessengerProtocol::sendCommand(SESSION_ESTABLISHED, NULL,
						peer);

				//removing peers from dispatcher list
				peers.erase(std::remove(peers.begin(), peers.end(), peer),
						peers.end());
				peers.erase(std::remove(peers.begin(), peers.end(), targetPeer),
						peers.end());
				handler->managePeerSession(peer, targetPeer);
				break;
			}
			default: {
				cout << "got command:" << command << " from peer: "
						<< peer->fromAddr() << endl << "peer disconnected: "
						<< peer->fromAddr() << endl;
				peer->close();
				peers.erase(std::remove(peers.begin(), peers.end(), peer),
						peers.end());
				delete peer;
				break;
			}

			}
		}

	}
	cout << "dispatcher ended" << endl;

}
void Dispatcher::listPeers() {
	vector<TCPSocket*>::iterator iter = peers.begin();
	for (; iter != peers.end(); iter++) {
		TCPSocket* peer = *iter;
		cout << peer->fromAddr() << endl;
	}
}
void Dispatcher::close() {
	this->isRunning = false;
	this->waitForThread();

	vector<TCPSocket*>::iterator iter = peers.begin();
	for (; iter != peers.end(); iter++) {
		TCPSocket* peer = *iter;
		peer->close();
		delete peer;
	}
	this->peers.clear();
}

TCPSocket* Dispatcher::findAvailablePeer(const char* peerAddr) {
	vector<TCPSocket*>::iterator iter = peers.begin();
	for (; iter != peers.end(); iter++) {
		TCPSocket* sock = *iter;
		if (strcmp(sock->fromAddr().c_str(), peerAddr) == 0) {
			return sock;
		}
	}
	return NULL;

}

} /* namespace npl */
