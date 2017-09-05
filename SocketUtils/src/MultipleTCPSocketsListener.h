/*
 * MultipleTCPSocketsListener.h
 *
 *  Created on: Sep 5, 2017
 *      Author: user
 */

#ifndef SRC_MULTIPLETCPSOCKETSLISTENER_H_
#define SRC_MULTIPLETCPSOCKETSLISTENER_H_

#include <iostream>
#include <stdio.h>
#include <vector>
#include "TCPSocket.h"
#include "Semaphore.h"

using namespace std;
namespace npl{

class MultipleTCPSocketsListener{
	// add class properties which include a vector (this will be the socket list) to hold the given sockets
	vector<TCPSocket*> *socketList;

	Semaphore* sem;

public:
	MultipleTCPSocketsListener();

	/*
	 * Add the given socket to the socket list to be listen on
	 */
	void addSocket(TCPSocket* socket);

	/*
	 * Set the given sockets to the socket list to be listen on
	 */
	void addSockets(vector<TCPSocket*> socketVec);

	/*
	 * This method checks to see if any of the previous given sockets are ready for reading
	 * It returns one of the Sockets that are ready.
	 */
	TCPSocket* listenToSocket();

	void close();

	~MultipleTCPSocketsListener();
};

}

#endif /* SRC_MULTIPLETCPSOCKETSLISTENER_H_ */
