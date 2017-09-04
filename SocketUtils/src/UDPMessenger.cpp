//============================================================================
// Name        : UDPMessenger
// Author      : Eliav Menachi
// Version     :
// Copyright   : Your copyright notice
// Description : UDP Messenger application
//============================================================================

#include "UDPMessenger.h"

using namespace std;
namespace npl{

void UDPMessenger::run() {

	char buff[1500];

	while (running) {
		int rc = udpSocket->recv(buff, 1500);
		if (rc > 0 && rc < 1500) {
			connectedUser=true;
			//changed = true;
			buff[rc] = 0;
			cout << "\nreceive msg from:" << udpSocket->fromAddr() << endl
					<< ">\"" << buff << "\"" << endl;
			//	this->SetNumber(s);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          );
		}
	}
	connectedUser=false;
	cout << "closing receiver thread" << endl;
}

UDPMessenger::UDPMessenger() {
	running = true;
	connectedUser=false;
	udpSocket = new UDPSocket(MSNGR_CLIENT_PORT);
	this->start();

}
bool UDPMessenger::isConnectedToUser(){
	return this->connectedUser != NULL;
}
void UDPMessenger::sendTo(string msg, string ip) {
	connectedUser=true;
	udpSocket->sendTo(msg, ip, MSNGR_CLIENT_PORT);
}

void UDPMessenger::reply(string msg) {
	udpSocket->reply(msg);
}

void UDPMessenger::close() {
	connectedUser=false;
	running = false;
	udpSocket->close();
	this->waitForThread();
	delete udpSocket;
}
}
