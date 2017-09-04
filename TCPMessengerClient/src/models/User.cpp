/*
 * User.cpp
 *
 *  Created on: Aug 31, 2017
 *      Author: user
 */

#include "User.h"
namespace npl {
User::User(string userName, string ip) {
	this->userName = userName;
	this->ip = ip;
	this->port = MSNGR_CLIENT_PORT;
}
User::User(string userName) {
	this->userName = userName;
	this->ip = "no ip";
	this->port = MSNGR_CLIENT_PORT;
}
User::User() {
	this->userName = "no name";
	this->ip = "no ip";
	this->port = MSNGR_CLIENT_PORT;
}
string User::getIP() {
	return this->ip;
}

string User::getUserName() {
	return this->userName;
}

User::~User() {
}

}
