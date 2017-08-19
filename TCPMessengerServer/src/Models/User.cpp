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
	this->listeningPort = 0;
}

User::User(string userName, string ip, int port, int listeningPort){
	this->userName = userName;
	this->socket = NULL;
	this->connectedUser = NULL;
	this->busy = false;
	this->listeningPort = listeningPort;
}

TCPSocket* User::getSocket(){
	return this->socket;
}

string User::getIP(){
	return this->socket->getIP();
}

int User::getPort(){
	return this->socket->getPort();
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
}

void User::setListeningPort(int port){
	this->listeningPort = port;
}

int User::getListeningPort(){
	return this->listeningPort;
}

User::~User() {
	this->socket->close();
	delete this->socket;
	this->socket = NULL;
}

