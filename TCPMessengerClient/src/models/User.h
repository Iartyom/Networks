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
#define MSNGR_CLIENT_PORT 10000
using namespace std;
namespace npl {
class User {
	string userName;
	string ip;
	int port;

public:
	User(string userName, string ip);
	User(string userName);
	User();
	virtual ~User();

	string getUserName();
	string getIP();
};
}
#endif /* USERC_H_ */
