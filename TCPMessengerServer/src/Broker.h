/*
 * Broker.h
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#ifndef SRC_BROKER_H_
#define SRC_BROKER_H_
#include "TCPSocket.h"
#include "MThread.h"
#include "MTCPListener.h"

namespace npl {

class BrokerMng;
class Broker: public MThread {
	BrokerMng* parent;
	TCPSocket* peer1;
	TCPSocket* peer2;
	bool isRunning;
public:
	Broker(TCPSocket* peer1, TCPSocket* peer2, BrokerMng* parent);
	virtual void run();
	void close();
};

} /* namespace npl */

#endif /* SRC_BROKER_H_ */
