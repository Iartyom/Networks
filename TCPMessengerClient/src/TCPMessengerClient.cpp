/*
 * TCPMessengerClient.cpp
 *
 *  Created on: 18 Jun 2017
 *      Author: oren
 */

#include <arpa/inet.h>
#include "TCPMessengerClient.h"
#include "TCPMessengerProtocol.h"

using namespace std;
namespace npl {

TCPMessengerClient::TCPMessengerClient() {
	socket = NULL;
	isListeningToMessages = false;
	isSessionOpen = false;
}

TCPMessengerClient::~TCPMessengerClient() {
	if (socket != NULL) {
		delete socket;
	}
}

//listen to messages from server with thread
void TCPMessengerClient::run() {
	this->isListeningToMessages = true;
	while (isListeningToMessages) {
		int cmd;
		char data[1024];
		if (TCPMessengerProtocol::readCommand(cmd, data, this->socket) == -1) {
			cout << "communication with server was interrupted - connection closed" << endl;
			this->isListeningToMessages = false;
			this->socket->close();
			delete this->socket;
			this->socket = NULL;
			return;
		}

		switch (cmd) {
		case SESSION_ESTABLISHED:
			cout << "Session was established with remote peer" << endl;
			isSessionOpen = true;
			break;
		case SESSION_REFUSED:
			cout << SESSION_REFUSED_MSG << endl;
			isSessionOpen = false;
			break;
		case SEND_MSG_TO_PEER:
			cout << "<<" << data << endl;
			break;
		case CLOSE_SESSION_WITH_PEER:
			cout << "session was closed by remote peer" << endl;
			isSessionOpen = false;
			break;
		case OPEN_SESSION_WITH_PEER:
			cout << "session was opened by remote peer: " << data << endl;
			isSessionOpen = true;
			break;
		case EXIT:
			cout << "connection with server interrupted" << endl;
			break;
		}
	}
}

bool TCPMessengerClient::connect(string ip) {
	cout << "opening a new client socket" << endl;
	socket = new TCPSocket(ip, MSNGR_PORT);
	this->start();
	return true;
}

bool TCPMessengerClient::openSession(string ipAndPort) {
	if (isSessionOpen) {
		cout << "Another session is opened, closing it" << endl;
		closeSession();
	}
	cout << "Opening session with: " << ipAndPort << endl;
	int res = TCPMessengerProtocol::sendCommand(OPEN_SESSION_WITH_PEER, ipAndPort.data(), this->socket);
	isSessionOpen = true;
	return res > -1;
}

void TCPMessengerClient::sendMessage(string msg) {
	TCPMessengerProtocol::sendCommand(SEND_MSG_TO_PEER, msg.data(), this->socket);
}

void TCPMessengerClient::closeSession() {
	if (TCPMessengerProtocol::sendCommand(CLOSE_SESSION_WITH_PEER, NULL, this->socket) > -1) {
		cout << "session closed sent successfully" << endl;
		isSessionOpen = false;
		return;
	}
	cout << "session close did not sent successfully" << endl;
	return;
}

void TCPMessengerClient::disconnect() {
	if (socket == NULL) {
		cout << "client is already disconnected" << endl;
		return;
	}
	TCPMessengerProtocol::sendCommand(EXIT, NULL, this->socket);

	//close and release all session related object objects
	this->isListeningToMessages = false;
	this->waitForThread();
	this->socket->close();
	delete this->socket;
	this->socket = NULL;
	cout << "client has been disconnected" << endl;

}


} /* namespace npl */
