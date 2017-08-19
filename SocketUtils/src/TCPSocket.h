/*
 * TCPSocket.h
 *
 *  Created on: 29 Apr 2017
 *      Author: oren
 */

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


using namespace std;

namespace npl {
class TCPSocket {
	int sock;
	sockaddr_in localAddr;
	sockaddr_in peerAddr;
private:
	/*
	 * private constructor to create a secondary server socket to communicate with a remote peer
	 */
	TCPSocket(int connected_sock,struct sockaddr_in serverAddr,struct sockaddr_in peerAddr);

public:
	/*
	 * Constructor create a TCP server socket
	 */
	TCPSocket(int port);

	/*
	 * constructor to creates TCP client socket and connect it to the remote peer in the given ip and port
	 */
	TCPSocket(string peerIp, int port);
	/*
	 * Perform listen and accept on server socket,
	 * this function returns a new TCPSocket object to communicate over the incoming session
	 */
	TCPSocket* listenAndAccept();

	/*
	 * Read from socket into the given buffer up to the buffer given length.
	 * return the number of bytes read
	 */
	int recv(char* buffer, int length);

	/*
	 * send the given buffer to the socket
	 */
	int send(const char* msg, int len);

	/*
	 * close the socket and free all resources
	 */
	void close();

	/*
	 * return the address of the connected peer
	 */
	string fromAddr();

	/**
	 * return the socket_fd
	 */
	int socket();

	string getIP();

	int getPort();
};
}
#endif /* TCPSOCKET_H_ */
