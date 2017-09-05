/*
 * UsersManager.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#include "UsersManager.h"

namespace npl {
	
UsersManager::UsersManager(UsersRepository *usersRepository, LoginHandler* handler)
{
	this->loginHandler = handler;
	this->mutex = PTHREAD_MUTEX_INITIALIZER;
	this->usersRepository = usersRepository;
	this->notLoggedInPeers = new vector<TCPSocket *>();
	this->loggedInUsers = new vector<User *>();
	this->isRunning = false;
}

void UsersManager::run()
{
	while (isRunning)
	{	
		if (notLoggedInPeers->size() == 0) {
			isRunning = false;
			break;
		}
		MTCPListener listener;
		Guard guard(&mutex);
		listener.add(*notLoggedInPeers);
		guard.dispose();
		TCPSocket *peer = listener.listen(5);
		if (peer == NULL)
		{
			continue;
		}

		int command = TCPMessengerProtocol::readCommand(peer);
		string userName;
		string password;
		switch (command)
		{
		case LOGIN:
			userName = TCPMessengerProtocol::readData(peer);
			password = TCPMessengerProtocol::readData(peer);
			if (!this->login(userName, password))
			{
				cout << "failed to logged in: " << userName << endl;
				TCPMessengerProtocol::sendCommand(peer, LOGIN_FAILED);
			}
			else
			{
				cout << "successfully logged in user: " << userName << endl;
				TCPMessengerProtocol::sendCommand(peer, LOGIN);
				this->addToLoggedIn(userName, peer);
				
			}
			break;
		case REGISTER:
			userName = TCPMessengerProtocol::readData(peer);
			password = TCPMessengerProtocol::readData(peer);
			if (!this->registerUser(userName, password))
			{
				cout << "failed to register : " << userName << endl;
				TCPMessengerProtocol::sendCommand(peer, REGISTER_FAILED);
			}
			else
			{
				cout << "successfully registered user: " << userName << endl;
				TCPMessengerProtocol::sendCommand(peer, REGISTER);
				this->addToLoggedIn(userName, peer);
			}
			break;
		default:
			cout << "not login command, closing session: " << peer->fromAddr() << endl;
			TCPMessengerProtocol::sendCommand(peer, EXIT);
			peer->close();
			this->removePeer(peer);
			break;
		}
	}
}

void UsersManager::addPeer(TCPSocket *peer)
{
	Guard guard(&mutex);
	this->notLoggedInPeers->push_back(peer);
	if (!isRunning) {
		isRunning = true;
		start();
	}
	
}

void UsersManager::removePeer(TCPSocket *peer)
{
	Guard guard(&mutex);
	vector<TCPSocket *>::iterator it = this->notLoggedInPeers->begin();
	for (; it != this->notLoggedInPeers->end(); ++it)
	{
		if ((*it) == peer)
		{
			this->notLoggedInPeers->erase(it);
			break;
		}
	}
}

void UsersManager::addToLoggedIn(string userName, TCPSocket *peer)
{
	this->removePeer(peer);
	Guard guard(&mutex);	
	this->loggedInUsers->push_back(new User(userName, peer));
	this->loginHandler->newUserLoggedIn();
}

vector<User *> *UsersManager::getLoggedInUsers()
{
	return this->loggedInUsers;
}

vector<std::pair<string, int>> UsersManager::getScoreboard(){
	return this->usersRepository->getOrderedScoreboard();
}

bool UsersManager::isUserLoggedIn(string userName)
{
	Guard guard(&mutex);	
	vector<User *>::iterator it = this->loggedInUsers->begin();
	for (; it != this->loggedInUsers->end(); ++it)
	{
		if ((*it)->getUserName().compare(userName) == 0)
		{
			return true;
		}
	}
	return false;
}

bool UsersManager::registerUser(string userName, string password)
{
	if (this->usersRepository->isUserExist(userName))
	{
		return false;
	}
	this->usersRepository->registerUser(userName, password);
	return true;
}

bool UsersManager::login(string userName, string password)
{
	if (!this->usersRepository->isUserExist(userName))
	{ // the user not exist
		return false;
	}
	if (this->isUserLoggedIn(userName))
	{ // the user already connected
		return false;
	}
	return this->usersRepository->login(userName, password);
}

void UsersManager::logout(User *user)
{	try{
		TCPMessengerProtocol::sendCommand(user->getSocket(), EXIT);
	} catch(...){
		
	}
	
	Guard guard(&mutex);
	vector<User *>::iterator it = this->loggedInUsers->begin();
	for (; it != this->loggedInUsers->end(); ++it)
	{
		if ((*it) == user)
		{
			delete (*it);
			this->loggedInUsers->erase(it);
			return;
		}
	}
	cout<< "user disconnected: "<<user->getUserName()<<endl;
}

void UsersManager::close()
{
	this->isRunning = false;
	this->waitForThread();

	vector<TCPSocket *>::iterator it = this->notLoggedInPeers->begin();
	for (; it != this->notLoggedInPeers->end(); ++it)
	{
		(*it)->close();
	}
	this->notLoggedInPeers->clear();

	vector<User *>::iterator it2 = this->loggedInUsers->begin();
	for (; it2 != this->loggedInUsers->end(); ++it2)
	{
		delete (*it2);
	}
	this->loggedInUsers->clear();
}

void UsersManager::addScore(User* user, int score){
	this->usersRepository->addScore(user->getUserName(), score);
}

UsersManager::~UsersManager()
{
	delete this->notLoggedInPeers;
	this->notLoggedInPeers = NULL;

	delete this->loggedInUsers;
	this->loggedInUsers = NULL;
}
}
