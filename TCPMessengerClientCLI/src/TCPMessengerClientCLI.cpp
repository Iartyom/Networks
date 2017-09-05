//============================================================================
// Name        : TCPMessengerClientCLI.cpp
// Author      : oren
// Version     :
// Copyright   : oren
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "MessengerClient.h"

using namespace npl;
using namespace std;

void printInstructions() {
	cout << "c <IP> - connect to the server in the given ip" << endl;
	//cout << "lu - print the user list from the server" << endl;
	cout << "lcu - print the connected users list" << endl;

	cout << "login <user> <password> - login with the user and password"
			<< endl;
	cout
			<< "register <user> <password> - register the new user with the given password and login the user."
			<< endl;
	cout << "o <username> - open a game with the user" << endl;

	/*	cout << "s <message> - send a message" << endl;*/
	cout
			<< "l - print the current status of the client (connected to “xyz”/not connected)"
			<< endl;
	cout << "cs - disconnect the open game " << endl;
	cout << "d - disconnect from server" << endl;
	cout << "x - close the app" << endl;
}

bool handleCommand(string command, MessengerClient* client) {
	if (command == "c") {
		string ip;
		cin >> ip;
		/*if (client->validateConnectedServer(true))*/
		client->connect(ip);
	} /*else if (command == "lu") {
	 if (validateConnected(client))
	 client->printAllUsers();
	 }*/else if (command == "lcu") {
		/*if (client->validateConnectedServer())*/
		client->printConnectedUsers();
	}/* else if (command == "lr") {
	 if (validateConnected(client))
	 client->printAllRooms();
	 } else if (command == "lru") {
	 string roomName;
	 cin >> roomName;
	 if (validateConnected(client))
	 client->printRoomUsers(roomName);
	 }*/else if (command == "login") {
		client->handleLogin();
	} else if (command == "register") {
		client->handleRegister();
	} else if (command == "o") {
		string userName;
		cin >> userName;
		if (client->validateLoggedIn())
			client->startGameRequest(userName);
	}/* else if (command == "or") {
	 string roomName;
	 cin >> roomName;
	 if (validateConnected(client) && validateLoggedIn(client))
	 client->enterRoom(roomName);user_target
	 }*//* else if (command == "s") {
	 string message;
	 getline(cin, message);
	 if (validateConnected(client) && validateLoggedIn(client)
	 && validateActiveSession(client))
	 client->send(message);
	 }*/else if (command == "l") {
		client->printStatus();
	} else if (command == "cs") {
		if (client->validateActiveSession())
			client->closeActiveGame();
	} else if (command == "d") {
		/*	if (client->validateConnectedServer())*/
		client->disconnect();
	} else if (command == "x") {
		if (client->isActiveGame() != NULL) {
			client->closeActiveGame();
		}
		if (client->isConnected() != NULL) {
			client->disconnect();
		}
		delete client;
		return false;
	} else if ((command == "yes") && (client->getRequestedForGame()==true)) {
		client->handleAccept();

	} else if ((command == "no") && (client->getRequestedForGame()==true)) {
		client->handleReject();
	} else {
		cout << "wrong input" << endl;
		printInstructions();
	}
	return true;
}

int main() {
	cout << "Welcome to client" << endl;
	printInstructions();
	MessengerClient* client = new MessengerClient();
	bool running = true;
	while (running) {
		string msg;
		string command;
		if (client->isActiveGame()==false)
		cin >> command;

		running = handleCommand(command, client);

	}
	client->disconnect();
	delete client;
	cout << "client was closed" << endl;
	return 0;
}
/*
 int main() {
 cout << __cplusplus << endl;
 cout << "Welcome to TCP Messenger Client" << endl;
 printInstructions();
 TCPMessengerClient* client = new TCPMessengerClient();
 bool isRunning = true;
 // read user input and act accordingly
 while (isRunning) {
 string instruction;
 string param;
 cin >> instruction;
 if (instruction == "x") {
 isRunning = false;
 } else if (instruction == "c") {
 cin >> param;
 client->connect(param);
 } else if (instruction == "o") {
 cin >> param;
 client->openSession(param);
 } else if (instruction == "cs") {
 client->closeSession();
 } else if (instruction == "d") {
 client->disconnect();
 } else if (instruction == "s") {
 cin >> param;
 client->sendMessage(param);
 } else {


 cout << "invalid instruction" << endl;
 }
 }

 // when done, close and delete all objects
 client->disconnect();
 delete client;
 cout << "client closed" << endl;
 }*/

