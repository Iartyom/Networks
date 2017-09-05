/*
 * User.h
 *
 *  Created on: Aug 31, 2017
 *      Author: user
 */

#ifndef USERC_H_
#define USERC_H_
#include <string>
//#include "TCPMessengerProtocol.h"

using namespace std;
namespace npl {
class User {
	string userName;
	string ip;
	int port;
	int listening_port;

public:
	User(string userName, string ip,int port);
	User(string userName);
	User();
	virtual ~User();

	string getUserName();
	string getIP();
	int getPort();
	void setPort(int p);
	int getListeningPort();
};
}
#endif /* USERC_H_ */
