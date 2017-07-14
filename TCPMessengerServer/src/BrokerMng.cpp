/*
 * BrokerMng.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#include "BrokerMng.h"
#include <string.h>
#include <algorithm>

namespace npl {

BrokerMng::BrokerMng(BrokerMngHandler* handler) {
	this->handler = handler;
}

void BrokerMng::createBroker(TCPSocket* peer1, TCPSocket* peer2) {
	cout << "create new broker between:" << peer1->fromAddr() << " and "
			<< peer2->fromAddr() << endl;
	Broker* broker = new Broker(peer1, peer2, this);
	this->brokers.push_back(broker);
}
;

void BrokerMng::releasePeer(TCPSocket* peer) {
	handler->handleReturnedPeer(peer);
}
void BrokerMng::deleteBroker(Broker* broker) {

	brokers.remove(broker);

	cout << "broker deleted, left: " << brokers.size() << endl;
}

void BrokerMng::close() {
	//closing broker will also delete from list
	while (!brokers.empty()){
		brokers.front()->close();
	}
}

} /* namespace npl */
