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

class GameApp:public MThread ,UDPMessengerGameHandler {
private:
	bool game_over = false;
	int next_number = 0;
	bool first_number = false;
	string boom = "boom";
	string msg;
	string command;
	string ip;
	int cur_number;
	UDPMessenger* messenger;
	bool win ;
	bool closing,should_boom;
	int listeningPort,enemyPort;
public:

	ConsoleLinux console;
	GameApp();
	void run();
	void PrintInstructions();
	void RunGame(string IP,int port,int listening_port);
	bool numberBoom(int number);
	void signalHandler(int signum);
	virtual ~GameApp();
	bool isInsideGame();
	void closeGameMessanger();
	
	virtual void gameEnded();

	bool getWin();
};

#endif /* GAMEAPP_H_ */
