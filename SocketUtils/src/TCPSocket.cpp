/*
 * TCPSocket.cpp
 *
 *  Created on: 29 Apr 2017
 *      Author: oren
 */

#include "TCPSocket.h"

namespace npl {

TCPSocket::TCPSocket(int peer_sock, struct sockaddr_in serverAddr,
		struct sockaddr_in peerAddr) {
	this->sock = peer_sock;
	this->localAddr = serverAddr;
	this->peerAddr = peerAddr;
}

TCPSocket::TCPSocket(int port) {
	int sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("Error opening channel");
		::close(sock);
		this->sock=-1;
		return;
	}

	int optval = 1;

	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		perror("setsockopt(SO_REUSEADDR) failed");
	#ifdef SO_REUSEPORT
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0)
			perror("setsockopt(SO_REUSEPORT) failed");
	#endif
	struct sockaddr_in serv_name;
	bzero(&serv_name, sizeof(serv_name));
	serv_name.sin_family = AF_INET;
	serv_name.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *) &serv_name, sizeof(serv_name)) < 0) {
		perror("Error naming channel");
		::close(sock);
		this->sock=-1;
		return;
	}

	this->sock = sock;
	this->localAddr = serv_name;
}

TCPSocket::TCPSocket(string peerIp, int port) {
	struct sockaddr_in peerAddr;
	int sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("Error opening channel");
		::close(sock);
		this->sock=-1;
		return;
	}
	bzero(&peerAddr, sizeof(peerAddr));
	peerAddr.sin_family = AF_INET;
	peerAddr.sin_addr.s_addr = inet_addr(peerIp.c_str());
	peerAddr.sin_port = htons(port);
	if (connect(sock, (struct sockaddr *) &peerAddr, sizeof(peerAddr)) < 0) {
		perror("Error establishing communications");
		::close(sock);
		this->sock=-1;
		return;
	}

	this->sock=sock;
	this->peerAddr=peerAddr;
}

TCPSocket* TCPSocket::listenAndAccept() {

	//start listening on socket
	listen(this->sock, 1);

	struct sockaddr_in peerAddr;
	unsigned int len = sizeof(peerAddr);

	//waiting for peer socket to connect
	int connected_peer_socket = accept(this->sock,
			(struct sockaddr *) (&peerAddr), &len);
	//return new TCPSocket object representing the connected peer

	if(connected_peer_socket>-1){
		return new TCPSocket(connected_peer_socket, this->localAddr, peerAddr);
	}
	return NULL;
}

int TCPSocket::recv(char* buffer, int length) {
	int byteRead = ::read(this->sock,buffer,length);
	return byteRead;
}

int TCPSocket::send(const char* msg, int len) {
	int byteSent = write(this->sock, msg, len);
	return byteSent;
}

void TCPSocket::close() {
	cout<<"closing socket"<<endl;
	shutdown(this->sock, SHUT_RDWR);
	::close(this->sock);
}

string TCPSocket::fromAddr() {

	string seperator = ":";
	string ip = inet_ntoa(this->peerAddr.sin_addr);

	char portBuff[50];
	memset((char*)portBuff,'\0', 50);
	sprintf(portBuff,"%i", this->peerAddr.sin_port);
	string port(portBuff);
	return  ip + seperator + port;
}
int TCPSocket::getPort(){
	return this->peerAddr.sin_port;
}

string TCPSocket::getIP(){
	return inet_ntoa(this->peerAddr.sin_addr);
}
int TCPSocket::socket(){
	return this->sock;
}
}
