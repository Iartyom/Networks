/*
 * UsersRepository.h
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#ifndef USERSREPOSITORY_H_
#define USERSREPOSITORY_H_

class UsersRepository {
public:
	UsersRepository();
	virtual ~UsersRepository();
	bool validatePassword(string userName,string password);
	bool registerUser(string userName, string password);
	vector<string> getAllUsers();
	bool isUserExist(string userName);
};

#endif /* USERSREPOSITORY_H_ */
