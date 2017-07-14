/*
 * LoginManager.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#include "LoginManager.h"

LoginManager::LoginManager(LoginHandler* handler, UsersRepository* usersRepository) {
	this->usersRepository = usersRepository;
	this->notLoggedInPeers = new vector<TCPSocket*>();
	this->loggedInUsers = new vector<User*>();
	this->isRunning = false;

	this->start();
}

void LoginManager::run(){
	this->isRunning = true;
	while(this->isRunning){
		MTCPListener listener;
		//sem->lock();
		listener->add(*this->notLoggedInPeers);
		//sem->unlock();
		TCPSocket* readySock = listener->listen();
		if(readySock == NULL){
			continue;
		}

		int command = TCPMessanger::readCommand(readySock);
		string userName;
		string password;
		switch(command){
			case LOGIN:
				userName = TCPMessanger::readData(readySock);
				password = TCPMessanger::readData(readySock);
				if(!login(userName, password)){
					cout<<"failed to logged in: " << userName << endl;
					TCPMessanger::sendCommand(readySock, LOGIN_FAILED);
				}
				else{
					cout << "successfully logged in user: " << userName << endl;
					TCPMessanger::sendCommand(readySock, LOGIN);
					this->addToLoggedIn(userName, readySock);
				}
				break;
			case REGISTER:
				userName = TCPMessanger::readData(readySock);
				password = TCPMessanger::readData(readySock);
				if(!this->registerUser(userName, password)){
					cout << "failed to register : " << userName << endl;
					TCPMessanger::sendCommand(readySock, REGISTER_FAILED);
				}
				else{
					cout << "successfully registered user: " << userName << endl;
					TCPMessanger::sendCommand(readySock, REGISTER);
					this->addToLoggedIn(userName, readySock);
				}
				break;
			default:
				cout<< "not login command, closing session: " << readySock->fromAddr()<<endl;
				TCPMessanger::sendCommand(readySock, EXIT);
				readySock->cclose();
				this->removePeer(readySock);
				break;
		}
	}
}

void LoginManager::addPeer(TCPSocket* peer){
	sem->lock();
	this->notLoggedInPeers->push_back(peer);
	sem->unlock();
}

void LoginManager::removePeer(TCPSocket* peer){
	sem->lock();
	vector<TCPSocket*>::iterator it = this->notLoggedInPeers->begin();
	for(;it != this->notLoggedInPeers->end(); ++it){
		if((*it) == peer){
			this->notLoggedInPeers->erase(it);
			break;
		}
	}
	sem->unlock();
}

void LoginManager::addToLoggedIn(string userName, TCPSocket* peer){
	this->removePeer(peer);
	sem->lock();
	this->loggedInUsers->push_back(new User(userName, peer));
	sem->unlock();
}

vector<User*>* LoginManager::getLoggedInUsers(){
	return this->loggedInUsers;
}

bool LoginManager::isUserLoggedIn(string userName){
	sem->lock();
	vector<User*>::iterator it = this->loggedInUsers->begin();
	for(; it != this->loggedInUsers->end(); ++it){
		if((*it)->getUserName().compare(userName) == 0){
			sem->unlock();
			return true;
		}
	}
	sem->unlock();
	return false;
}

bool LoginManager::registerUser(string userName, string password){
	if(this->usersRepository->isUserExist(userName)){
		return false;
	}
	ofstream usersFile;
	sem->lock();
	usersFile.open ("users.txt", std::ios_base::app);
	usersFile << userName << " "<<password<<endl;
	usersFile.close();
	sem->unlock();
	return true;
}

bool LoginManager::login(string userName, string password){
	if(!this->usersRepository->isUserExist(userName)){ // the user not exist
		return false;
	}
	if(this->isUserLoggedIn(userName)){ // the user already connected
		return false;
	}
	bool returnValue = false;
	string delimeter = " ";
	string line;
	sem->lock();
	ifstream usersFile("users.txt");
	if (usersFile.is_open())
	{
		while (usersFile.good())
		{
			getline(usersFile, line);
			if (line.compare(userName + delimeter + password)==0)
			{
				returnValue = true;
				break;
			}
		}
	}
	usersFile.close();
	sem->unlock();
	return returnValue;
}

void LoginManager::logout(User* user){
	sem->lock();
	vector<User*>::iterator it = this->loggedInUsers->begin();
	for(; it != this->loggedInUsers->end(); ++it){
		if((*it) == user){
			delete (*it);
			this->loggedInUsers->erase(it);
			sem->unlock();
			return;
		}
	}
	sem->unlock();
}

void LoginManager::close(){
	this->isRunning = false;
	this->waitForThread();

	vector<TCPSocket*>::iterator it = this->notLoggedInPeers->begin();
	for(;it!=this->notLoggedInPeers->end();++it){
		(*it)->cclose();
	}
	this->notLoggedInPeers->clear();

	vector<User*>::iterator it2 = this->loggedInUsers->begin();
	for(;it2!=this->loggedInUsers->end();++it2){
		delete (*it2);
	}
	this->loggedInUsers->clear();
}

LoginManager::~LoginManager() {
	delete this->notLoggedInPeers;
	this->notLoggedInPeers = NULL;

	delete this->loggedInUsers;
	this->loggedInUsers = NULL;

	delete this->sem;
	this->sem = NULL;
}

