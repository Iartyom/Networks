/*
 * UsersRepository.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#include "UsersRepository.h"

#include <sstream>
#include <algorithm>

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

			//split values of line by space

			string buf; 
			stringstream ss(line); 
		
			vector<string> values;

			while (ss >> buf)
				values.push_back(buf);

			if (values[0].compare(userName) == 0)
			{
				if(values[1].compare(std::to_string(str_hash(password))) == 0){
					returnValue = true;
				}
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
	usersFile << userName << " "<<str_hash(password)<<" "<<0<<endl;
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
		while (getline(usersFile, line))
		{
			if(line.empty()) continue;
			string delimiter = " ";
			string userName = line.substr(0, line.find(delimiter));
			users.push_back(userName);
		}
	}
	usersFile.close();
	return users;
}
vector<pair<string, int>> UsersRepository::getOrderedScoreboard(){
	vector<pair<string, int>> usersWithScores;
	string line;
	Guard guard(&mutex);	
	ifstream usersFile("users.txt");
	if (usersFile.is_open())
	{
		while (usersFile.good())
		{
			getline(usersFile, line);
			if(line.empty()) continue;
			//split values of line by space
			string buf; 
			stringstream ss(line); 
		
			vector<string> values;

			while (ss >> buf)
				values.push_back(buf);
			//push pair of <user, score>
			usersWithScores.push_back(std::make_pair( values[0], std::stoi(values[2]) ));
		}
	}
	usersFile.close();
	guard.dispose();

	std::sort(usersWithScores.begin(), usersWithScores.end(), scoreComparator);
	return usersWithScores;
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

bool UsersRepository::addScore(string userName, int score){
	bool returnValue = false;
	Guard guard(&mutex);
	std::fstream file("users.txt", std::ios::in);
	
	if(file.is_open()) {
		std::string line;
		std::vector<std::string> lines;
	
		while(std::getline(file, line)) {
	
			string delimiter = " ";
			string name = line.substr(0, line.find(delimiter));
			if(name.compare(userName)==0){

				//split values of line by space
				string buf; 
				stringstream ss(line); 
			
				vector<string> values;
	
				while (ss >> buf)
					values.push_back(buf);

				line = values[0]+ delimiter + values[1] + delimiter + 
					std::to_string(std::stoi(values[2]) + score);

				returnValue=true;

			}
			//std::cout << line << std::endl;
			lines.push_back(line);
		}
	
		file.close();
		file.open("users.txt", std::ios::out | std::ios::trunc);
	
		for(const auto& i : lines) {
			file << i << std::endl;
		}
		file.close();
	}
	return returnValue;
}

UsersRepository::~UsersRepository()
{
	// TODO Auto-generated destructor stub
}
}
