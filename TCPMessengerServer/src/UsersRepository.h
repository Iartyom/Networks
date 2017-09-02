/*
 * UsersRepository.h
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#ifndef USERSREPOSITORY_H_
#define USERSREPOSITORY_H_

#include "Guard.h"
#include <pthread.h>
#include <fstream>
#include <iostream>
#include <tr1/functional>
#include <string>
#include <vector>
using namespace std;

namespace npl {
	
class UsersRepository {
	std::tr1::hash<std::string> str_hash;
	pthread_mutex_t mutex;	
public:
	UsersRepository();
	virtual ~UsersRepository();
	bool login(string userName, string password);
	bool registerUser(string userName, string password);
	void addScore(string userName, int score);
	vector<string> getAllUsers();
	bool isUserExist(string userName);
};
}
#endif /* USERSREPOSITORY_H_ */
