/*
 * User.cpp
 *
 *  Created on: Aug 31, 2017
 *      Author: user
 */

#include "User.h"
namespace npl {
User::User(string userName, string ip,int port) {
	this->userName = userName;
	this->ip = ip;
	this->port = port;
}
void User::setPort(int p){
	this->port = p;
}
User::User(string userName) {
	this->userName = userName;
	this->ip = "no ip";
	this->port = 0;
}
User::User() {
	this->userName = "no name";
	this->ip = "no ip";
	this->port = 0;
}
string User::getIP() {
	return this->ip;
}
int User::getPort() {
	return this->port;
}
int User::getListeningPort() {
	return this->listening_port;
}
string User::getUserName() {
	return this->userName;
}

User::~User() {
}

}
