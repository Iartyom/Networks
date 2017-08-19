/*
 * UsersRepository.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */
#include <string.h>
#include "UsersRepository.h"
using namespace std;
namespace npl {
	
UsersRepository::UsersRepository()
{
	this->mutex = PTHREAD_MUTEX_INITIALIZER;
}
bool UsersRepository::login(string userName, string password)
{
	bool returnValue = false;
	string delimeter = " ";
	string line;
	Guard guard(&mutex);	
	ifstream usersFile("users.txt");
	if (usersFile.is_open())
	{
		while (usersFile.good())
		{
			getline(usersFile, line);
			if (line.compare(userName + delimeter + str_hash(password)) == 0)
			{
				returnValue = true;
				break;
			}
		}
	}
	usersFile.close();
	return returnValue;
}
bool UsersRepository::registerUser(string userName, string password)
{
	ofstream usersFile;
	Guard guard(&mutex);		
	usersFile.open ("users.txt", std::ios_base::app);
	usersFile << userName << " "<<str_hash(password)<<endl;
	usersFile.close();
	return true;
}
vector<string> UsersRepository::getAllUsers()
{
	vector<string> users;
	string line;
	Guard guard(&mutex);
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
	return users;
}
bool UsersRepository::isUserExist(string userName)
{
	vector<string> users = this->getAllUsers();
	Guard guard(&mutex);
	vector<string>::iterator it = users.begin();
	for (; it != users.end(); ++it)
	{
		if ((*it).compare(userName) == 0)
		{
			return true;
		}
	}
	return false;
}

void UsersRepository::addScore(string userName, int score)

UsersRepository::~UsersRepository()
{
	// TODO Auto-generated destructor stub
}
}