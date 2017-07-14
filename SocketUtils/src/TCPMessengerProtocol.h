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

#define CLOSE_SESSION_WITH_PEER 	1
#define OPEN_SESSION_WITH_PEER 		2
#define EXIT						3
#define SEND_MSG_TO_PEER			4
#define SESSION_REFUSED				5
#define SESSION_ESTABLISHED			6

#define TEST_PEER_NAME "test"
#define SESSION_REFUSED_MSG "Connection to peer refused, peer might be busy or disconnected, try again later"

namespace npl{
class TCPMessengerProtocol{
public:
	int static sendCommand(int cmd, const char* data, TCPSocket* socket) {
		int cmdNet = htonl(cmd);
		int res = socket->send((char*) &cmdNet, 4);
		if (res < 4) {
			return -1;
		}
		if (data != NULL) {
			int len = strlen(data);
			int lenNet = htonl(len);
			res = socket->send((char*) &lenNet, 4);
			if (res < 4) {
				return -1;
			}
			res = socket->send(data, len);
			if (res < len) {
				return -1;
			}
		}
		return res;
	}
	int static readCommand(int& cmd, char* data, TCPSocket* socket) {
		int receivedcmd;
		int res = socket->recv((char*) &receivedcmd, 4);
		if (res < 4) {
			return -1;
		}
		cmd = ntohl(receivedcmd);
		switch (cmd) {
		case SEND_MSG_TO_PEER:
		case OPEN_SESSION_WITH_PEER:
			int len;
			res = socket->recv((char*) &len, 4);
			if (res < 4) {
				return -1;
			}
			len = ntohl(len);
			res = socket->recv(data, len);
			if (res < 4) {
				return -1;
			}
			data[len] = '\0';
			return res;
		}
		return 0;

	}
};
}

#endif /* TCPMESSENGERPROTOCOL_H_ */
