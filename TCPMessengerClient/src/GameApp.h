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
using namespace npl;



class GameApp {
public:
	GameApp();
	void PrintInstructions();
	void RunGame(string IP);
	bool numberBoom(char* number);
	void signalHandler(int signum);
	virtual ~GameApp();

};

#endif /* GAMEAPP_H_ */
