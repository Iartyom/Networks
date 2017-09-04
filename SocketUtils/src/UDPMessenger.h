//============================================================================
// Name        : UDPMessenger.h
// Author      : Eliav Menachi
// Version     :
// Copyright   : Your copyright notice
// Description : UDP Messenger application
//============================================================================
#ifndef MThread_H_
#define MThread_H_

#include <pthread.h>
#include "UDPSocket.h"
#include <mutex>
#include "MThread.h"
#include "TCPMessengerProtocol.h"
using namespace std;
namespace npl{

class UDPMessenger: public MThread {
	//TODO: declare the class properties
	UDPSocket* udpSocket;
	bool running;
	bool connectedUser;
	string number;

public:
	bool isConnectedToUser();
	void SetNumber(string s) {
		this->number = s;
	}
	;

	string GetNumber() {
		return this->number;
	}
	;
	UDPMessenger();
	/**
	 * sends the given message to the given peer specified by IP
	 */
	void sendTo(string msg, string ip);

	/**
	 * reply to an incoming message, this method will send the given message
	 * the peer from which the last message was received.
	 */
	void reply(string msg);

	/**
	 * close the messenger and all related objects (socket)
	 */
	void close();

	/**
	 * This method runs in a separate thread, it reads the incoming messages
	 * from the socket and prints the content on the terminal.
	 * The thread should exist when the socket is closed
	 */
	void run();
};
}
#endif
