/*
 * TCPMessengerProtocol.h
 *
 *  Created on: Feb 13, 2013
 *      Author: efi
 */

#ifndef TCPMESSENGERPROTOCOL_H_
#define TCPMESSENGERPROTOCOL_H_

#include "TCPSocket.h"
#include <string.h>

/**
 * TCP Messenger protocol:
 * all messages are of the format [Command 4 byte int]
 * and optionally data follows in the format [Data length 4 byte int][ Data ]
 */
#define MSNGR_PORT 3346
#define MSNGR_CLIENT_PORT 10000

#define REQUEST_TO_START_GAME 		1 //data - name of user. if 
#define START_GAME 					14 //data - port to listen + target user details
#define REQUEST_TO_START_RANDOM_GAME 		20
#define GAME_REQUEST_REJECTED 		2 //data - name of user
#define GAME_REQUEST_ACCEPTED 		3 //data - name of user
#define SESSION_REFUSED				7

#define GAME_ENDED 					4 
#define GAME_CANCEL 				5

#define EXIT						6

#define LOGIN						8
#define LOGIN_FAILED				9
#define REGISTER					10
#define REGISTER_FAILED				11

#define GET_ONLINE_USERS_LIST		12 //data - number of users + (foreach user: userName + status)
#define GET_SCOREBOARD 				13

#define TEST_USER_NAME "test"
#define SESSION_REFUSED_MSG "Connection to peer refused, peer might be busy or disconnected, try again later"


//Game Results

#define LOSE						21
#define WIN						22

#define DRAW						23
#define CANCELLED					24

namespace npl{

class TCPMessengerProtocol{
public:
	static void sendCommand(TCPSocket* peer, int command){
		cout << "sending command " <<command << endl;
		int commandNumber = htonl(command);
		peer->send((char*)&commandNumber,4);
	}

	static void sendData(TCPSocket* peer, string msg){
		int dataLength = htonl(msg.length());
		peer->send((char*)&dataLength, 4);
		peer->send(msg.data(), msg.length());
	}

	static void sendInt(TCPSocket* peer, int number){
		number = htonl(number);
		peer->send((char*)&number,4);
	}

	static int readCommand(TCPSocket* peer){
		cout << "reading command ";

		int commandType;
		int rc = peer->recv((char*)&commandType, 4);
		if(rc != 4){
			return -1;
		}
		commandType = ntohl(commandType);
		cout << commandType<<endl;
		return commandType;
	}

	static int readInt(TCPSocket* peer){
		int number;
		int rc = peer->recv((char*)&number, 4);
		if(rc != 4){
			return -1;
		}
		number = ntohl(number);
		return number;
	}

	static string readData(TCPSocket* peer){
		int dataLen;
		int rc = peer->recv((char*)&dataLen,4);
		if(rc != 4){
			return "";
		}
		dataLen = ntohl(dataLen);
		char buffer[dataLen + 1];
		memset((char*)buffer,'\0',dataLen + 1);
		peer->recv(buffer, dataLen);
		string data = string(buffer);
		return data;
	}
};
}

#endif /* TCPMESSENGERPROTOCOL_H_ */
