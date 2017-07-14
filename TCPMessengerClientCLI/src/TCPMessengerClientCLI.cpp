//============================================================================
// Name        : TCPMessengerClientCLI.cpp
// Author      : oren
// Version     :
// Copyright   : oren
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "TCPMessengerClient.h"

using namespace npl;
using namespace std;

void printInstructions() {

	cout << "To open connection to server: c <server ip>>" << endl;
	cout << "To open session with peer: o <peer ip:port>" << endl;
	cout << "Opening session will close any previously active sessions" << endl;
	cout << "After the session is opened, to send message type: s <message>"
			<< endl;
	cout << "To close open session type: cs" << endl;
	cout << "To disconnect type: d" << endl;
	cout << "To exit type: x" << endl;
}

int main() {
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
}

