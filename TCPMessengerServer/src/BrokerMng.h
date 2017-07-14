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
	virtual void handleReturnedPeer(TCPSocket* peer)=0;
	virtual ~BrokerMngHandler(){}
};

class BrokerMng {
	BrokerMngHandler* handler;
	list<Broker*> brokers;
public:
	BrokerMng(BrokerMngHandler* handler);
	void createBroker(TCPSocket* peer1, TCPSocket* peer2);

	void releasePeer(TCPSocket* peer);
	void deleteBroker(Broker* broker);
	void close();
};

} /* namespace npl */

#endif /* SRC_BROKERMNG_H_ */
