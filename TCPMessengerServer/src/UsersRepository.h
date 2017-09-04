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
	struct UsersScoreComperator
	{
		bool operator()(std::pair<string,int> const & a, std::pair<string,int> const & b){
			return a.second != b.second?  a.second > b.second : a.first > b.first;
		}
	}scoreComparator;
public:
	UsersRepository();
	virtual ~UsersRepository();
	bool login(string userName, string password);
	bool registerUser(string userName, string password);
	bool addScore(string userName, int score);
	vector<string> getAllUsers();
	vector<std::pair<string, int>> getOrderedScoreboard();
	bool isUserExist(string userName);
};
}
#endif /* USERSREPOSITORY_H_ */
