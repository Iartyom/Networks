/*
 * GameApp.cpp
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#include "GameApp.h"

GameApp::GameApp() {
	// TODO Auto-generated constructor stub

}

void GameApp::PrintInstructions(){
	cout<<"The Game"<<endl;
	cout<<"the name of the game is 7 BOOM "<<endl;
	cout<<"each player counts until he reaches a number witch has 7 in it "<<endl;
	cout<<"or a number witch is a multiplication of 7 and then writes boom"<<endl;
	cout<<"the first one to miss a boom loses "<<endl;
	cout<<"To send a message type: s <message>"<<endl;
	cout<<"To exit type: x"<<endl;
}

bool GameApp::numberBoom(char* number){
	int num =atoi(number);
	int thisDigit;
	if(number%7 ==0)
		return 1;
	while (number != 0)
	{

		thisDigit = number % 10;    // Always equal to the last digit of thisNumber
	    number = number / 10;   // Always equal to thisNumber with the last digit
	                                    // chopped off, or 0 if thisNumber is less than 10
	    if (thisDigit == 7)
	    {
	        return 1;
	    }
	}
	return 0;
}

void GameApp::RunGame(string enemyIP){
	cout<<"Welcome to The Game 7 Boom messenger"<<endl;
		PrintInstructions();
		UDPMessenger* messenger = new UDPMessenger();
		while(true){
			string msg;
			string command;
			cin >> command;
			if(command == "s"){
				if(msg.size()>0 && msg[0] == ' ') msg.erase(0,1);
				messenger->sendTo(msg, enemyIP);
			}else if(command == "x"){
				break;
			}else{
				cout<<"wrong input"<<endl;
				PrintInstructions();
			}
		}
		messenger->close();
		delete messenger;
		cout<<"messenger was closed"<<endl;
}

void signalHandler( int signum ) {
   cout << "Interrupt signal (" << signum << ") received.\n";

   // cleanup and close up stuff here
   // terminate program

   exit(signum);

}

GameApp::~GameApp() {
	// TODO Auto-generated destructor stub
}

