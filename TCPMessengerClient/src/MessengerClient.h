/*
 * MessengerClient.h
 *
 *  Created on: Aug 31, 2017
 *      Author: user
 */

#ifndef MESSENGERCLIENT_H_
#define MESSENGERCLIENT_H_

#include <string.h>
#include <vector>
#include "MThread.h"
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "models/User.h"
#include "UDPMessenger.h"
#include "TCPMessengerProtocol.h"
#include "GameApp.h"

using namespace std;
namespace npl {

class MessengerClient: public MThread {
	TCPSocket* serverSocket ;
	//UDPMessenger* udpMessenger;
	User* currentUser; // the user object after login
	/*
	 string connectingUserName;
	 string connectingRoomName;
	 */
	bool isHandlingServerData;

	bool isRunning;
private:
	User* readUser();
	//Room* readRoom();
	void handle();
//	vector<User*> getAllUsers();

	vector<User*> getConnectedUsers();
	GameApp *game;// = new GameApp;
	User *opponent;// = new User;

	static User* readUser(TCPSocket* sock) {
		string userName = TCPMessengerProtocol::readData(sock);
		string userIp = TCPMessengerProtocol::readData(sock);
		return new User(userName, userIp);
	};

//	void printUsers(vector<User*> users);
public:
	MessengerClient();
	virtual ~MessengerClient();

	bool validateConnectedServer(bool reverse = false);
	bool validateLoggedIn(bool reverse = false);
	bool validateActiveSession();
	void handleLogin();
	void handleRegister();
	void printStatus();

	bool isLoggedIn();
	bool isConnected();
	void login(string userName, string password);
	void registerUser(string userName, string password);
	void connect(string ip);
	void disconnect();
//	void connectUser(string userName);
//	void enterRoom(string roomName);
	void closeActiveSession(bool remote = false);
	bool isActiveSession();
	bool isConnectedWithUser();
//	bool isConnectedWithRoom();
	User* getActiveSessionUser();
	//Room* getActiveSessionRoom();
	void send(string message);

	void startGameRequest(string user_target);
//	string getUserIPByUserName(string user_target);
//	string get_OpponentName();
	//void printAllRooms();
//	void printAllUsers();
	void printConnectedUsers();
//	void printRoomUsers(string roomName);
	User* getCurrentUser();

	void run();
	void close();
};
}
#endif /* MESSENGERCLIENT_H_ */
