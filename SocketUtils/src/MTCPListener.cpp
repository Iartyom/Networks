/*
 * MTCPListener.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: user
 */

#include "MTCPListener.h"

namespace npl {
void MTCPListener::add(TCPSocket* socket) {
	sockets.push_back(socket);

}

void MTCPListener::add(vector<TCPSocket*> sockets) {
	this->sockets = sockets;
}

TCPSocket* MTCPListener::listen(int timeoutSeconds=0) {
	fd_set set;
	int nfd = 0;
	FD_ZERO(&set);
	tSockets::iterator iter = sockets.begin();
	for (; iter != sockets.end(); iter++) {
		TCPSocket* sock = *iter;
		int fd = sock->socket();
		FD_SET(fd, &set);
		if (fd >= nfd) {
			nfd = fd + 1;
		}
	}

	int rc;
	if(timeoutSeconds!=0){
		struct timeval timeout;
			timeout.tv_sec = timeoutSeconds;
			timeout.tv_usec = 0;
			rc = select(nfd, &set, NULL, NULL, &timeout);
	}
	else{
		rc = select(nfd, &set, NULL, NULL, NULL);
	}


	if (rc < 1) {
		FD_ZERO(&set);
		return NULL;
	}

	iter = sockets.begin();
	for (; iter != sockets.end(); iter++) {
		TCPSocket* sock = *iter;
		int fd = sock->socket();
		if (FD_ISSET(fd, &set)) {
			FD_ZERO(&set);
			return sock;
		}
	}
	FD_ZERO(&set);
	return NULL;
}

void MTCPListener::close() {
	this->sockets.clear();
}

} /* namespace npl */
