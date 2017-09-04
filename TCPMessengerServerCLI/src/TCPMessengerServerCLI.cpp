//============================================================================
// Name        : TCPMessengerServerCLI.cpp
// Author      : oren
// Version     :
// Copyright   : oren
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "TCPMSNServer.h"
using namespace std;
using namespace npl;
void printInstructions(){
	cout<<"-----------------------"<<endl;
	cout<<"lu - list users"<<endl;
	cout<<"lo - list online users"<<endl;
	cout<<"ls - list scoreboard"<<endl;
	cout<<"x - exit"<<endl;
	
	cout<<"-----------------------"<<endl;
}
int main() {
	cout<<"Welcome to TCP messenger Server"<<endl;
		printInstructions();
		TCPMSNServer msngrServer;
		while(true){
			string msg;
			string command;
			cin >> command;
			if(command == "lu"){
				msngrServer.printAllUsers();
			}else if(command == "lo"){
				msngrServer.printConnectedUsers();
			}else if(command == "ls"){
				msngrServer.printScoreboard(); 
			}else if(command == "x"){
				break;
			}else{
				cout<<"wrong input"<<endl;
				printInstructions();
			}
		}
		msngrServer.exit();
		cout<<"messenger was closed"<<endl;
		return 0;
}
