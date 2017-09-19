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
bool firstCinAfterGame = false;
void printInstructions() {
	cout << " menu - to remember the menu" << endl;
	cout << "c <IP> - connect to the server in the given ip" << endl;

	cout << "lcu - print the connected users list" << endl;

	cout << "login <user> <password> - login with the user and password"
			<< endl;
	cout << "register <user> <password> - register a new protected user."
			<< endl;
	cout << "game <username> - open a game with the user" << endl;
	cout << "random - open a random game" << endl;

	cout << "stat - print the current status of the client" << endl;
	cout << "scoreboard - print scoreboard " << endl;

	cout << "d - disconnect from server" << endl;
	cout << "x - close the app" << endl;
}

bool handleCommand(string command, MessengerClient* client) {
	if (command == "c") {
		string ip;
		cin >> ip;

		client->connect(ip);
	} else if (command == "lcu") {

		client->printConnectedUsers();
	} else if (command == "menu") {
		printInstructions();
	} else if (command == "login") {
		client->handleLogin();
	} else if (command == "register") {
		client->handleRegister();
	} else if (command == "game") {
		firstCinAfterGame = true;
		string userName;
		cin >> userName;
		if (client->validateLoggedIn())
			client->startGameRequest(userName);
	} else if (command == "random") {
		if (client->validateLoggedIn())
			client->startRandomGame();
	} else if (command == "stat") {
		client->printStatus();
	} else if (command == "cs") {
		if (client->isActiveGame())
			client->closeActiveGame();
	} else if (command == "scoreboard") {
		client->handleScore();
	} else if (command == "d") {
		/*	if (client->validateConnectedServer())*/
		client->disconnect();
	} else if (command == "x") {
		if (client->isActiveGame()) {
			client->closeActiveGame();
		}
		if (client->isConnected()) {
			client->disconnect();
		}
		delete client;
		return false;
	} else if ((command == "yes") && (client->getRequestedForGame() == true)) {
		firstCinAfterGame = true;
		client->handleAccept();


	} else if ((command == "no") && (client->getRequestedForGame() == true)) {
		client->handleReject();
	} else {
		cout << "wrong input" << endl;
		printInstructions();
	}
	return true;
}

int main() {
	string command;

	cout << "Welcome to client" << endl;
	printInstructions();
	MessengerClient* client = new MessengerClient();
	bool running = true;

	while (running) {
		//string msg;
		if (client->isActiveGame()
				&& (client->GameMassengerActive() == false)) {
			client->gameEnded();
			printInstructions();

		} else if (client->isActiveGame()) {
			sleep(1);
			continue;
		}
		else if(client->pendingRequest){
			sleep(1);
			continue;
		} else {

			if (!firstCinAfterGame) {
				cout<<"waiting for command"<<endl;

				cin >> command;
				running = handleCommand(command, client);
			} else{
				firstCinAfterGame = false;
			}
		}

	}
	client->disconnect();
	delete client;
	cout << "client was closed" << endl;
	return 0;
}
