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

void BrokerMng::createBroker(User* user1, User* user2) {
	cout << "create new broker between:" << user1->fromAddr() << " and "
			<< user2->fromAddr() << endl;
	Broker* broker = new Broker(user1, user2, this);
	this->brokers.push_back(broker);
}
;

void BrokerMng::gameEnded(User* user1, User* user2, GAME_RESULT result) {
	handler->handleGameEnded(peer);
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
