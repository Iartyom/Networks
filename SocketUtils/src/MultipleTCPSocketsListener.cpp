/*
 * MultipleTCPSocketsListener.cpp
 *
 *  Created on: Sep 5, 2017
 *      Author: user
 */

#include "MultipleTCPSocketsListener.h"

using namespace std;
namespace npl{

MultipleTCPSocketsListener::MultipleTCPSocketsListener() {
	this->socketList = new vector<TCPSocket*>();
	this->sem = new Semaphore(1);
}

/*
 * Add the given socket to the socket list to be listen on
 */
void MultipleTCPSocketsListener::addSocket(TCPSocket* socket) {
	sem->lock();
	// add he given socket to the socket list
	this->socketList->push_back(socket);
	sem->unlock();
}

/*
 * Set the given sockets to the socket list to be listen on
 */
void MultipleTCPSocketsListener::addSockets(vector<TCPSocket*> socketVec) {
	sem->lock();
	// set the given sockets as the socket list to select from
	this->socketList->insert(this->socketList->begin(), socketVec.begin(),
			socketVec.end());
	sem->unlock();
}

/*
 * This method checks to see if any of the previous given sockets are ready for reading
 * It returns one of the Sockets that are ready.
 */
TCPSocket* MultipleTCPSocketsListener::listenToSocket() {
	if (this->socketList->size() == 0) {
		return NULL;
	}
	// create local set for the select function (fd_set)
	fd_set set;
	FD_ZERO(&set);

	sem->lock();
	// fill the set with file descriptors from the socket list using (FD_SET macro)
	for (vector<TCPSocket*>::iterator it = this->socketList->begin();
			it != this->socketList->end(); ++it) {
		FD_SET((*it)->socket(), &set);
	}
	sem->unlock();

	// perform the select
	struct timeval time = { 5, 0 };
	int result = select(sizeof(set) * 8, &set, NULL, NULL, &time);

	// check the returned value from the select to find the socket that is ready
	if (result == -1) {
		perror("select failed");
		return NULL;
	}
	if (result == 0) { // no waiting sockets...
		return NULL;
	}
	sem->lock();
	for (vector<TCPSocket*>::iterator it = this->socketList->begin();
			it != this->socketList->end(); ++it) {
		if (FD_ISSET((*it)->socket(), &set)) {
			sem->unlock();
			return (*it);
		}
	}
	sem->unlock();

	// if select return a valid socket return the matching TCPSocket object otherwise return NULL
	return NULL;
}

void MultipleTCPSocketsListener::close() {
	sem->lock();
	this->socketList->clear();
	sem->unlock();
}

MultipleTCPSocketsListener::~MultipleTCPSocketsListener() {
	if (this->sem == NULL)
		return;
	sem->lock();
	delete this->socketList;
	this->socketList = NULL;
	sem->unlock();

	delete this->sem;
	this->sem = NULL;
}
}
