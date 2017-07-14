/*
 * UsersRepository.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#include "UsersRepository.h"

UsersRepository::UsersRepository()
{
	// TODO Auto-generated constructor stub
}
bool UsersRepository::validatePassword(string userName, string password)
{
}
bool UsersRepository::registerUser(string userName, string password)
{
}
vector<string> UsersRepository::getAllUsers()
{
	vector<string> users;
	string line;
	sem->lock();
	ifstream usersFile("users.txt");
	if (usersFile.is_open())
	{
		while (usersFile.good())
		{
			getline(usersFile, line);
			string delimiter = " ";
			string userName = line.substr(0, line.find(delimiter));
			users.push_back(userName);
		}
	}
	usersFile.close();
	sem->unlock();
	return users;
}
bool UsersRepository::isUserExist(string userName)
{
	vector<string> users = this->getAllUsers();
	sem->lock();
	vector<string>::iterator it = users.begin();
	for (; it != users.end(); ++it)
	{
		if ((*it).compare(userName) == 0)
		{
			sem->unlock();
			return true;
		}
	}
	sem->unlock();
	return false;
}

UsersRepository::~UsersRepository()
{
	// TODO Auto-generated destructor stub
}
