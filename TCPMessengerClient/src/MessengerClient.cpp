/*
 * MessengerClient.cpp
 *
 *  Created on: Aug 31, 2017
 *      Author: user
 */

//#include "TCPMessengerProtocol.h"
#include "MessengerClient.h"
#define WIN_SCORE 2
#define LOSE_SCORE 1
namespace npl {
MessengerClient::MessengerClient() {
	this->currentUser = NULL;
	//this->udpMessenger = NULL;
	this->serverSocket = NULL;
	/*this->connectingUserName = "";
	 this->connectingRoomName = "";*/
	this->isHandlingServerData = false;
	this->requested_for_game = false;
	this->msngrProtocol = new TCPMessengerProtocol();
	game = NULL;
	opponent = NULL;
	this->isRunning = false;
}
// ----------------------------------------------- handle from outside + checks----------------------------
void MessengerClient::handleLogin() {
	string userName;
	string password;
	cin >> userName;
	cin >> password;
//	if (/*!validateConnectedServer() ||*/ !validateLoggedIn(true))
//		return;

	this->login(userName, password);
}

void MessengerClient::handleRegister() {
	string userName;
	string password;
	cin >> userName;
	cin >> password;
	if (/*!validateConnectedServer() ||*/!validateLoggedIn(true))
		return;

	this->registerUser(userName, password);
}

bool MessengerClient::isLoggedIn() {
	return this->currentUser != NULL;
}

bool MessengerClient::isConnected() {
	return this->serverSocket != NULL && this->serverSocket->socket() > 0;
}

bool MessengerClient::validateLoggedIn(bool reverse) {
	if (!reverse) {
		if (!this->isLoggedIn()) {
			cout << "you are not logged in! you must login first!" << endl;
			return false;
		}
		return true;
	}
	if (this->isLoggedIn()) {
		cout << "you are already logged in as: "
				<< this->getCurrentUser()->getUserName() << endl;
		return false;
	}
	return true;
}
// ----------------------------------------------- server connection handle----------------------------
void MessengerClient::registerUser(string userName, string password) {
	this->isHandlingServerData = true;
	TCPMessengerProtocol::sendCommand(this->serverSocket, REGISTER);
	TCPMessengerProtocol::sendData(this->serverSocket, userName);
	TCPMessengerProtocol::sendData(this->serverSocket, password);

	//now we wait for the server to answer if the register succeeded
	int command = TCPMessengerProtocol::readCommand(this->serverSocket);
	if (command == REGISTER) {
		this->currentUser = new User(userName); // the ip and port is not relevant for our user..
		cout << "Register succeeded, you are now logged as: " << userName
				<< " :) " << endl;
	} else {
		cout
				<< "Register failed, please try again, your user name is probably already in use by other user"
				<< endl;
	}
	this->isHandlingServerData = false;
}

void MessengerClient::connect(string ip) {
	this->serverSocket = new TCPSocket(ip, MSNGR_PORT);
	if (!(this->serverSocket->socket() > 0)) {
		this->serverSocket = NULL;
		cout << "failed to connect.. try again" << endl;
		return;
	}
	cout << "connected!!" << endl;
	this->start();
}
void MessengerClient::login(string userName, string password) {
	this->isHandlingServerData = true;

	TCPMessengerProtocol::sendCommand(this->serverSocket, LOGIN);
	TCPMessengerProtocol::sendData(this->serverSocket, userName);
	TCPMessengerProtocol::sendData(this->serverSocket, password);
	int command = msngrProtocol->readCommand(this->serverSocket);
	//this->isHandlingServerData = true;
	//now we wait for the server to answer if the login succeeded
	if (command == LOGIN) {
		this->currentUser = new User(userName); // the ip and port is not relevant for our user..
		cout << "Login succeeded, you are now logged as: " << userName << " :) "
				<< endl;
	} else {
		cout
				<< "are you in already? ;)	OR Login failed, please try again, your user name or password are incorrect"
				<< endl;
	}
	this->isHandlingServerData = false;
}
void MessengerClient::disconnect() {
	this->closeActiveGame(true);

	this->close(); // stop the run loop that listening to the server socket...

	if (this->currentUser != NULL) {
		delete this->currentUser;
		this->currentUser = NULL;
	}

	if (this->serverSocket != NULL) {
		TCPMessengerProtocol::sendCommand(this->serverSocket, EXIT);
		this->serverSocket->close();
		delete this->serverSocket;
		this->serverSocket = NULL;
	}
	cout << "disconnected from server" << endl;
}
// ----------------------------------------------- server options----------------------------
void MessengerClient::printStatus() {
	string status = "STATUS: ";
	if (!this->isConnected()) {
		status += "not connected to a server";
	} else {
		status += "connected";
		if (this->isLoggedIn()) {
			status += (", logged in as: "
					+ this->getCurrentUser()->getUserName());
			if (this->isActiveGame()) {
				status += (", in active session "
						+ this->opponent->getUserName());

			}
		}
	}
	cout << status<<endl;
}
User* MessengerClient::getCurrentUser() {
	return this->currentUser;
}
void MessengerClient::handleScore() {
	this->isHandlingServerData = true;
	TCPMessengerProtocol::sendCommand(this->serverSocket, GET_SCOREBOARD);
	// now we wait for the server to answer with the connected users list
	int command = TCPMessengerProtocol::readCommand(this->serverSocket);
	if (command == GET_SCOREBOARD) {
		int scoreLength = TCPMessengerProtocol::readInt(this->serverSocket);
		for (int i = 0; i < scoreLength; i++) {
			cout << "User name: "
					<< TCPMessengerProtocol::readData(this->serverSocket);
			cout << ", Score: "
					<< TCPMessengerProtocol::readInt(this->serverSocket)
					<< endl;
		}
	} else {
		perror("Failed to get scoreboard list");
	}

	this->isHandlingServerData = false;
}
void MessengerClient::printConnectedUsers() {
	this->isHandlingServerData = true;

	cout << "Connected Users: " << endl;
//	vector<User*> users;
	TCPMessengerProtocol::sendCommand(this->serverSocket,
	GET_ONLINE_USERS_LIST);
	// now we wait for the server to answer with the connected users list
	int command = TCPMessengerProtocol::readCommand(this->serverSocket);
	if (command == GET_ONLINE_USERS_LIST) {
		int usersLength = TCPMessengerProtocol::readInt(this->serverSocket);
		for (int i = 0; i < usersLength; i++) {
			cout << "User name : "
					<< TCPMessengerProtocol::readData(this->serverSocket);
			cout << " Status : "
					<< TCPMessengerProtocol::readData(this->serverSocket)
					<< endl;
		}
	} else {
		perror("Failed to get connected users list");
	}

	this->isHandlingServerData = false;
}

bool MessengerClient::isActiveGame() {
	return this->game != NULL;
}

// ----------------------------------------------- game stuff----------------------------
//game start
void MessengerClient::startGameRequest(string user_target) {
	if(user_target==currentUser->getUserName()){
		cout<<"you can't game with yourself man"<<endl;
		return;
	}
	this->isHandlingServerData = true;
	TCPMessengerProtocol::sendCommand(this->serverSocket,
	REQUEST_TO_START_GAME);
	TCPMessengerProtocol::sendData(this->serverSocket, user_target);
	//now we wait for the server to answer with game approval
	this->isHandlingServerData = false;
}
void MessengerClient::startRandomGame() {
	cout<<"start random game"<<endl;
	this->isHandlingServerData = true;
	TCPMessengerProtocol::sendCommand(this->serverSocket,
			REQUEST_TO_START_RANDOM_GAME);
	this->isHandlingServerData = false;
}
void MessengerClient::startGame() {
	cout << "starting game function" << endl;
	this->isHandlingServerData = true;
	this->game = new GameApp();
	currentUser->setPort(TCPMessengerProtocol::readInt(this->serverSocket));
	opponent = MessengerClient::readUser(this->serverSocket);
	cout << "opening game by remote request with: " << opponent->getUserName()
			<< endl;
	game->RunGame(opponent->getIP(), opponent->getPort(),
			currentUser->getPort());
	this->isHandlingServerData = false;
}

bool MessengerClient::GameMassengerActive() {
	if (this->isActiveGame() == true) {
		if (this->game->isInsideGame() == false) {
			return false;
		}
		return true;
	}
	return true;
}
void MessengerClient::closeActiveGame(bool remote) {
	if (this->game != NULL) {
		delete this->game;
		this->game = NULL;
		if (remote) {
			cout << "session closed by remote user" << endl;
		}
	}
}
void MessengerClient::gameEnded() {
	TCPMessengerProtocol::sendCommand(this->serverSocket,
	GAME_ENDED);
	if (game->getWin()) {
		TCPMessengerProtocol::sendInt(this->serverSocket, WIN);
		TCPMessengerProtocol::sendInt(this->serverSocket, WIN_SCORE);
	} else {
		TCPMessengerProtocol::sendInt(this->serverSocket, LOSE);
		TCPMessengerProtocol::sendInt(this->serverSocket, LOSE_SCORE);
	}
	closeActiveGame(false);
	cout << "game closed" << endl;
}
void MessengerClient::handleAccept() {
	requested_for_game = false;
	TCPMessengerProtocol::sendCommand(this->serverSocket,
	GAME_REQUEST_ACCEPTED);
	TCPMessengerProtocol::sendData(this->serverSocket, potencialOpponent);
	potencialOpponent = "";
}
void MessengerClient::handleReject() {
	requested_for_game = false;
	TCPMessengerProtocol::sendCommand(this->serverSocket,
	GAME_REQUEST_REJECTED);
	TCPMessengerProtocol::sendData(this->serverSocket, potencialOpponent);
	potencialOpponent = "";
}

void MessengerClient::setUserAwnser(string s) {
	this->user_awnser = s;
}
bool MessengerClient::getRequestedForGame() {
	return this->requested_for_game;
}
void MessengerClient::close() {
	this->isRunning = false;
	this->waitForThread();
}
// ----------------------------------------------- main class funcs----------------------------
void MessengerClient::run() {

	this->isRunning = true;
	while (this->isRunning) {
		if (this->isHandlingServerData) {
			sleep(1);
			continue;
		}
		MTCPListener listener;
		listener.add(this->serverSocket);
		TCPSocket* readySocket = listener.listen(2);
		if (readySocket == NULL || this->isHandlingServerData) {
			continue;
		}
		handle();
	}
}

void MessengerClient::handle() {

	int command = TCPMessengerProtocol::readCommand(this->serverSocket);
	string userName;
	//User* user;
//	int listeningPort;
	//commands to execute

	switch (command) {

	case REQUEST_TO_START_GAME: {
		requested_for_game = true;
		cout << "received open game request from: ";
		//	listeningPort = TCPMessengerProtocol::readInt(this->serverSocket);
		potencialOpponent = TCPMessengerProtocol::readData(this->serverSocket);
		//	user = this->readUser();
		cout << potencialOpponent << endl;
		cout << "write 'yes' to accept or 'no' to decline" << endl;

	}
		break;
	case START_GAME: {
		cout << "!start game!" << endl;
		this->startGame();
	}
		break;
	case GAME_REQUEST_REJECTED: {
		cout
				<< "receiving opponent refusal , i think he hates you, or he's just busy"
				<< endl;
		//this->startGame();
	}
		break;
	case GAME_ENDED: {
		this->closeActiveGame(true);
	}
		break;
	case SESSION_REFUSED:{
		cout << "the user you requested is not online or there are not online users at all"<<endl;
		break;
	}
	case EXIT: {
		cout << "disconnect from the server by remote command" << endl;
		this->disconnect();
	}
		break;
	default: {
		break;
	}
	}
}

MessengerClient::~MessengerClient() {
}
}

