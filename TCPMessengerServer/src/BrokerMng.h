/*
 * BrokerMng.h
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#ifndef SRC_BROKERMNG_H_
#define SRC_BROKERMNG_H_

#include "TCPSocket.h"
#include "Broker.h"
#include <vector>
#include <list>

namespace npl {

class BrokerMngHandler{
public:
	virtual void handleGameEnded(TCPSocket* peer)=0;
	virtual ~BrokerMngHandler(){}
};

class BrokerMng {
	BrokerMngHandler* handler;
	list<Broker*> brokers;
public:
	BrokerMng(BrokerMngHandler* handler);
	void createBroker(User* user1, User* user2);

	void gameEnded(User* user1, User* user2, GAME_RESULT result)
	void deleteBroker(Broker* broker);
	void close();
};

} /* namespace npl */

#endif /* SRC_BROKERMNG_H_ */
