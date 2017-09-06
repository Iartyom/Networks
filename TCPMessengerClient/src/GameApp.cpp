/*
 * GameApp.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#include "GameApp.h"

GameApp::GameApp() {
	console = ConsoleLinux();
	closing = false;
	win = true;
}
void GameApp::run(){
	while (true) {
			//get command
		/*if (closing)
			break;*/
			cin >> command;
			if (command == "m") {
				//cin >> ip;
				getline(std::cin, msg);

				if (msg.size() > 0 && msg[0] == ' ')
					msg.erase(0, 1);
				//checks if the msg has digits or its not a legit game msg
				bool has_only_digits = (msg.find_first_not_of("0123456789")
						== string::npos);
				if (has_only_digits == false && msg.compare(boom) != 0) {
					cout
							<< "the message was sent but please enter youre number! \n";
					messenger->sendTo(msg, ip, enemyPort);
				} //its a part of the game , checks validation
				else {
					int num = atoi(msg.c_str());
					cur_number = num;
					if (first_number == false) {
						int num = atoi(msg.c_str());
						first_number = true;
						next_number = num;
					}
					should_boom = numberBoom(next_number);
					if (cur_number != next_number && msg.compare(boom) != 0) {
						cout << "wrong number, send the number in order \n";
					} else if ((should_boom == true && msg.compare(boom) != 0)
							|| (should_boom == false && msg.compare(boom) == 0)) {
						console.setColor(COL_RED);
						messenger->sendTo("BOOOOOOM!! you WIN i'm a loser \n", ip,
								enemyPort);
						messenger->sendTo("GAME OVER", ip, enemyPort);
						messenger->sendTo("end", ip, enemyPort);
						win = false;
						cout << "BOOOOOM!! damn i lost it \n";
						cout << "GAME OVER\n";
						cout << "Game was closed" << endl;
						//	console.clearScreen();
						//break;
						game_over = true;
						closeGameMessanger();
						break;
					} else {
						messenger->sendTo("my number is:" + msg, ip, enemyPort);
						next_number = next_number + 2;
					}
				}
			} else if (command == "e") {
				closeGameMessanger();
				break;
			} else if (command == "in") {
				PrintInstructions();
			} else {
				cout << "wrong input" << endl;
				PrintInstructions();
			}
		}
		//messenger->close();
		//delete messenger;
		cout << "messenger was closed" << endl;
}
void GameApp::PrintInstructions() {

	console.setColor(COL_GREEN);
	cout << "The Game - 7 BOOM " << endl;
	console.setColor(COL_MAGENTA);
	cout << "each player counts until he reaches a number witch has 7 in it "
			<< endl;
	cout << "or a number witch is a multiplication of 7 and then writes 'boom' "
			<< endl;
	cout << "the first one to miss a 'boom' loses " << endl;
	console.setColor(COL_BLUE);
	cout << "To send a message type: m <message>" << endl;
	cout << "for the instructions type : in" << endl;
	cout << "To exit type: e" << endl;
	//console.clearScreen();
}
/**
 * checks if the number is a number + checks if it should be BOOM
 */
bool GameApp::numberBoom(int number) {

//	int boomnum = atoi(number.c_str());
	int boomnum = number;
	int thisDigit;

	if (boomnum % 7 == 0)
		return true;
	while (boomnum != 0) {

		thisDigit = boomnum % 10; // Always equal to the last digit of thisNumber
		boomnum = boomnum / 10; // Always equal to thisNumber with the last digit
		// chopped off, or 0 if thisNumber is less than 10
		if (thisDigit == 7) {
			return true;
		}
	}
	return false;
}

void GameApp::RunGame(string enemyIP, int port, int listening_port) {
	//weclome + instracts
	this->ip = enemyIP;
	should_boom = false;
	cout << "Welcome to The Game 7 Boom messenger" << endl;
	PrintInstructions();
	this->enemyPort= port;
	this->listeningPort=listening_port;
	messenger = new UDPMessenger(listening_port, this);
	cout << "udp started" << endl;
	this->start();
	//main game UDP loop
	/*while (true) {
		//get command
		cin >> command;
		if (command == "m") {
			//cin >> ip;
			getline(std::cin, msg);

			if (msg.size() > 0 && msg[0] == ' ')
				msg.erase(0, 1);
			//checks if the msg has digits or its not a legit game msg
			bool has_only_digits = (msg.find_first_not_of("0123456789")
					== string::npos);
			if (has_only_digits == false && msg.compare(boom) != 0) {
				cout
						<< "the message was sent but please enter youre number! \n";
				messenger->sendTo(msg, ip, port);
			} //its a part of the game , checks validation
			else {
				int num = atoi(msg.c_str());
				cur_number = num;
				if (first_number == false) {
					int num = atoi(msg.c_str());
					first_number = true;
					next_number = num;
				}
				should_boom = numberBoom(next_number);
				if (cur_number != next_number && msg.compare(boom) != 0) {
					cout << "wrong number, send the number in order \n";
				} else if ((should_boom == true && msg.compare(boom) != 0)
						|| (should_boom == false && msg.compare(boom) == 0)) {
					console.setColor(COL_RED);
					messenger->sendTo("BOOOOOOM!! you WIN i'm a loser \n", ip,
							port);
					messenger->sendTo("GAME OVER", ip, port);
					messenger->sendTo("end", ip, port);
					win = false;
					cout << "BOOOOOM!! damn i lost it \n";
					cout << "GAME OVER\n";
					cout << "Game was closed" << endl;
					//	console.clearScreen();
					//break;
					game_over = true;
					closeGameMessanger();
					break;
				} else {
					messenger->sendTo("my number is:" + msg, ip, port);
					next_number = next_number + 2;
				}
			}
		} else if (command == "e") {
			closeGameMessanger();
			break;
		} else if (command == "in") {
			PrintInstructions();
		} else {
			cout << "wrong input" << endl;
			PrintInstructions();
		}
	}
	//messenger->close();
	//delete messenger;
	cout << "messenger was closed" << endl;*/
}

void GameApp::gameEnded(){
	//this->~MThread();
	//this->closing= true;
	closeGameMessanger();
}

bool GameApp::isInsideGame() {
	return this->messenger != NULL;
}
bool GameApp::getWin() {
	return win;
}
void GameApp::closeGameMessanger() {
	if (this->isInsideGame())
		messenger->close();
	delete messenger;
	this->messenger = NULL;
	cout << "Game was closed" << endl;
	console.setColor(COL_NORMAL);
}

void signalHandler(int signum) {
	cout << "Interrupt signal (" << signum << ") received.\n";
// cleanup and close up stuff here
// terminate program

	exit(signum);

}

GameApp::~GameApp() {
// TODO Auto-generated destructor stub
}

