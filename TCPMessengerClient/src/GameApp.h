/*
 * GameApp.h
 *
 *  Created on: Jul 14, 2017
 *      Author: user
 */

#ifndef GAMEAPP_H_
#define GAMEAPP_H_

#include "UDPMessenger.h"
#include <iostream>
#include <csignal>
#include <stdlib.h>
#include <pthread.h>

#include "ConsoleLinux.h"
using namespace npl;
using namespace std;

class GameApp {
private:
	bool game_over = false;
	int next_number = 0;
	bool first_number = false;
	string boom = "boom";
	string msg;
	string command;
	string ip;
	int cur_number;
public:

	ConsoleLinux console;
	GameApp();
	void PrintInstructions();
	void RunGame(string IP,int port,int listening_port);
	bool numberBoom(int number);
	void signalHandler(int signum);
	virtual ~GameApp();

};

#endif /* GAMEAPP_H_ */
