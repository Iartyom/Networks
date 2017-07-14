//============================================================================
// Name        : UDPMessenger
// Author      : Eliav Menachi
// Version     :
// Copyright   : Your copyright notice
// Description : UDP Messenger application
//============================================================================

#include "UDPMessenger.h"

using namespace std;
using namespace npl;


void UDPMessenger::signalHandler( int signum ) {

   exit(signum);

}
void UDPMessenger::run(){
	char buff[1500];
	signal(SIGINT, signalHandler);
	while(running){
		int rc = udpSocket->recv(buff,1500);
		if (rc>0 && rc<1500){
			buff[rc] = 0;
			cout<<"\nreceive msg from:"<<udpSocket->fromAddr()<<endl<<">\""<<buff<<"\""<<endl;
			this->returnedMessege = buff;
		}
	}
	cout<<"closing receiver thread"<<endl;
}

UDPMessenger::UDPMessenger(){
	running = true;
	udpSocket = new UDPSocket(MSNGR_PORT);
	this->start();
}

void UDPMessenger::sendTo(string msg,string ip){
	udpSocket->sendTo(msg,ip,MSNGR_PORT);
}

void UDPMessenger::reply(string msg){
	udpSocket->reply(msg);
}

void UDPMessenger::close(){
	running = false;
	udpSocket->close();
	this->waitForThread();
	delete udpSocket;
}

