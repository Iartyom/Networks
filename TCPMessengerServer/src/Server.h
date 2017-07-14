/*
 * Server.h
 *
 *  Created on: Jun 26, 2017
 *      Author: user
 */

#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_
#include "TCPSocket.h"
#include "MThread.h"

namespace npl {

class ServerHandler {
public:
	virtual void handlePeer(TCPSocket* peer)=0;
	virtual ~ServerHandler(){};
};

class Server : public MThread {
	ServerHandler* handler;
	TCPSocket* socket;
	bool isRunning;

public:
	Server(ServerHandler* handler);
	~Server(){};
	void run();
	void close();

};

} /* namespace npl */

#endif /* SRC_SERVER_H_ */
