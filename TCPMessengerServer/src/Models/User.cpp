/*
 * User.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#include "User.h"

User::User(string userName, TCPSocket* socket) {
	this->userName = userName;
	this->socket = socket;
	this->connectedUser = NULL;
	this->busy = false;
	this->initialIP = "";
}

int User::getUDPPort(){
	return this->port;
}
void User::setUDPPort(int port){
	this->port = port;
}
TCPSocket* User::getSocket(){
	return this->socket;
}

string User::getIP(){
	if(this->socket==NULL){
		return this->initialIP;
	}
	return this->socket->getIP();
}

string User::getUserName(){
	return this->userName;
}

bool User::isBusy(){
	return this->busy;
}

void User::setBusy(User* user){
	this->connectedUser = user;
	this->busy = true;
}

User* User::getConnectedUser(){
	return this->connectedUser;
}

void User::setAvailable(){
	this->busy = false;
	this->connectedUser = NULL;
	this->port = -1;
}

User::~User() {
	this->socket->close();
	delete this->socket;
	this->socket = NULL;
}

