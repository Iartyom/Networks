/*
 * Dispatcher.h
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#ifndef SRC_DISPATCHER_H_
#define SRC_DISPATCHER_H_
#include "TCPSocket.h"
#include "MThread.h"
#include <vector>

namespace npl {

class DispatcherHandler {
public:
	virtual void managePeerSession(TCPSocket* peer1, TCPSocket* peer2)=0;
	virtual ~DispatcherHandler(){}
};

class Dispatcher : public MThread {
	/**
	 * listen to all peers and handle the incoming commands from the peer

	 */
	DispatcherHandler* handler;
	vector<TCPSocket*> peers;

	bool isRunning;
private:
	TCPSocket* findAvailablePeer(const char* peerAddr);

public:
	Dispatcher(DispatcherHandler* handler);
	~Dispatcher(){}
	void addPeer(TCPSocket* peer);
	void run();
	void listPeers();
	void close();



};

} /* namespace npl */

#endif /* SRC_DISPATCHER_H_ */
