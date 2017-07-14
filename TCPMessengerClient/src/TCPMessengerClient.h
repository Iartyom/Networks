/*
 * TCPMessengerClient.h
 *
 *  Created on: 18 Jun 2017
 *      Author: oren
 */

#ifndef TCPMESSENGERCLIENT_H_
#define TCPMESSENGERCLIENT_H_

#include <pthread.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "MThread.h"
#include "TCPSocket.h"

using namespace std;
namespace npl{

class TCPMessengerClient : private MThread{
private:
	TCPSocket *socket;
	bool isListeningToMessages;
	bool isSessionOpen;

public:
	TCPMessengerClient();
	virtual ~TCPMessengerClient();

	/**
	 * connects the client to the remote server on the given ip,
	 * the server port is read from the protocol file
	 *
	 * @param ip - the ip of the server to connect to
	 *
	 * @return true on success otherwise false
	 */
	bool connect(string ip);

	bool openSession(string ipAndPort);

	void sendMessage(string msg);

	void run();

	void closeSession();

	void disconnect();


};
} /* namespace npl */

#endif /* TCPMESSENGERCLIENT_H_ */
