/*
 * MessengerClient.cpp
 *
 *  Created on: Aug 31, 2017
 *      Author: user
 */

//#include "TCPMessengerProtocol.h"
#include "MessengerClient.h"
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

bool MessengerClient::validateActiveSession() {
	if (!this->isActiveGame()) {
		cout << "You don't have active session" << endl;
		return false;
	}
	return true;
}

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
	cout << status;
}

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

bool MessengerClient::isLoggedIn() {
	return this->currentUser != NULL;
}

bool MessengerClient::isConnected() {
	return this->serverSocket != NULL && this->serverSocket->socket() > 0;
}

bool MessengerClient::isActiveGame() {
	return this->game != NULL;
}

User* MessengerClient::getCurrentUser() {
	return this->currentUser;
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
		cout << "disconnecting from server" << endl;
		this->disconnect();
	}

	this->isHandlingServerData = false;
}

//game start
void MessengerClient::startGameRequest(string user_target) {
	this->isHandlingServerData = true;
	TCPMessengerProtocol::sendCommand(this->serverSocket,
	REQUEST_TO_START_GAME);
	TCPMessengerProtocol::sendData(this->serverSocket, user_target);
	//now we wait for the server to answer with game approval
	this->isHandlingServerData = false;
}
void MessengerClient::startGame() {
	cout<<"starting game function"<<endl;
	this->isHandlingServerData = true;
	this->game=new GameApp();
	int listening_port = TCPMessengerProtocol::readInt(this->serverSocket);
	opponent = MessengerClient::readUser(this->serverSocket);
	cout << "opening game by remote request with: " << opponent->getUserName()
			<< endl;
	game->RunGame(opponent->getIP(),opponent->getPort(),currentUser->getPort());
	this->isHandlingServerData = false;
}
/*
 void MessengerClient::send(string msg) {
 if (this->udpMessenger == NULL) {
 cout << "no active session, unable to send message" << endl;
 return;
 }

 this->udpMessenger->sendMessage(msg);
 }
 */

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
void MessengerClient::closeActiveGame(bool remote) {
	if (this->game != NULL) {
		delete this->game;
		if (remote) {
			cout << "session closed by remote user" << endl;
		}
	}
}

void MessengerClient::run() {
	this->isRunning = true;
	while (this->isRunning) {
		if (this->isHandlingServerData) {
			sleep(1);
			continue;
		}
		MultipleTCPSocketsListener* listener = new MultipleTCPSocketsListener();
		listener->addSocket(this->serverSocket);
		TCPSocket* readySocket = listener->listenToSocket();
		listener->close();
		delete listener;
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
		potencialOpponent = TCPMessengerProtocol::readData(
				this->serverSocket);
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
			cout << "receiving opponent refusal , i think he hates you, or he's just busy" << endl;
			this->startGame();
		}
			break;
	case GAME_CANCEL: {
		this->closeActiveGame(true);
	}
		break;

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

MessengerClient::~MessengerClient() {
}
}
/*
 Room* MessengerClient::readRoom(){
 string roomName = TCPMessanger::readData(this->serverSocket);
 return new Room(roomName);
 }
 */
/*
 void MessengerClient::printAllRooms(){
 this->isHandlingServerData = true;

 cout<<"All Rooms: " << endl;
 TCPMessanger::sendCommand(this->serverSocket, GET_ROOMS_LIST);
 // now we wait for the server to answer with the rooms list
 int command = TCPMessanger::readCommand(this->serverSocket);
 if(command == GET_ROOMS_LIST){
 int roomsLength = TCPMessanger::readInt(this->serverSocket);
 for(int i = 0; i< roomsLength; i++){
 cout<< TCPMessanger::readData(this->serverSocket) << endl;
 }
 }
 else{
 perror("Failed to get rooms list");
 cout << "disconnecting from server" << endl;
 this->disconnect();
 }

 this->isHandlingServerData = false;
 }
 /*
 void MessengerClient::printAllUsers(){
 this->isHandlingServerData = true;

 cout<<"All Users: " << endl;
 TCPMessanger::sendCommand(this->serverSocket, GET_ONLINE_USERS_LIST);
 // now we wait for the server to answer with the users list
 int command = TCPMessanger::readCommand(this->serverSocket);
 if(command == GET_ONLINE_USERS_LIST){
 int usersLength = TCPMessanger::readInt(this->serverSocket);
 for(int i = 0; i< usersLength; i++){
 cout<< TCPMessanger::readData(this->serverSocket) << endl;
 }
 }
 else{
 perror("Failed to get users list");
 cout << "disconnecting from server" << endl;
 this->disconnect();
 }

 this->isHandlingServerData = false;
 }
 */
/*
 bool MessengerClient::isConnectedWithRoom(){
 return this->udpMessenger != NULL && this->udpMessenger->getConnectedRoom() != NULL;
 }

 bool MessengerClient::isConnectedWithUser(){
 return this->udpMessenger != NULL && this->udpMessenger->getConnectedUser() != NULL;
 }

 Room* MessengerClient::getActiveSessionRoom(){
 if(!this->isActiveSession()) return NULL;
 return this->udpMessenger->getConnectedRoom();
 }

 User* MessengerClient::getActiveSessionUser(){
 if(!this->isActiveSession()) return NULL;
 return this->udpMessenger->getConnectedUser();
 }
 */
/*
 void MessengerClient::printAllRooms(){
 this->isHandlingServerData = true;

 cout<<"All Rooms: " << endl;
 TCPMessanger::sendCommand(this->serverSocket, GET_ROOMS_LIST);
 // now we wait for the server to answer with the rooms list
 int command = TCPMessanger::readCommand(this->serverSocket);
 if(command == GET_ROOMS_LIST){
 int roomsLength = TCPMessanger::readInt(this->serverSocket);
 for(int i = 0; i< roomsLength; i++){
 cout<< TCPMessanger::readData(this->serverSocket) << endl;
 }
 }
 else{
 perror("Failed to get rooms list");
 cout << "disconnecting from server" << endl;
 this->disconnect();
 }

 this->isHandlingServerData = false;
 }
 /*
 void MessengerClient::printAllUsers(){
 this->isHandlingServerData = true;

 cout<<"All Users: " << endl;
 TCPMessanger::sendCommand(this->serverSocket, GET_ONLINE_USERS_LIST);
 // now we wait for the server to answer with the users list
 int command = TCPMessanger::readCommand(this->serverSocket);
 if(command == GET_ONLINE_USERS_LIST){
 int usersLength = TCPMessanger::readInt(this->serverSocket);
 for(int i = 0; i< usersLength; i++){
 cout<< TCPMessanger::readData(this->serverSocket) << endl;
 }
 }
 else{
 perror("Failed to get users list");
 cout << "disconnecting from server" << endl;
 this->disconnect();
 }

 this->isHandlingServerData = false;
 }
 */
/*
 bool MessengerClient::validateConnectedServer(bool reverse) {
 if (!reverse) {
 if (!this->isConnected()) {
 cout << "You must connect to a server first!" << endl;
 return false;
 }
 return true;
 }
 if (this->isConnected()) {
 cout << "You already connected to a server" << endl;
 return false;
 }
 return true;
 }*/
/*
 void MessengerClient::printRoomUsers(string roomName){
 this->isHandlingServerData = true;

 cout<<"Room: " << roomName << " Users: " << endl;
 TCPMessanger::sendCommand(this->serverSocket, GET_USERS_IN_ROOM);
 TCPMessanger::sendData(this->serverSocket, roomName);
 // now we wait for the server to answer with the room's users list
 int command = TCPMessanger::readCommand(this->serverSocket);
 if(command == GET_USERS_IN_ROOM){
 int usersLength = TCPMessanger::readInt(this->serverSocket);
 for(int i = 0; i< usersLength; i++){
 User* user = this->readUser();
 cout<< user->getUserName() << endl;
 delete user;
 }
 }
 else{
 perror("Failed to get room's users list");
 cout << "disconnecting from server" << endl;
 this->disconnect();
 }

 this->isHandlingServerData = false;
 }
 */
/*
 string MessengerClient::getUserIPByUserName(string user_target) {

 this->isHandlingServerData = true;

 cout << "getting user by name: " << user_target << endl;

 TCPMessengerProtocol::sendCommand(this->serverSocket, GET_USER);

 // now we wait for the server to answer with the connected users list
 int command = TCPMessanger::readCommand(this->serverSocket);
 if (command == GET_USER_IP) {
 return TCPMessanger::readData(this->serverSocket);

 } else {
 perror("Failed to get connected users list");
 cout << "disconnecting from server" << endl;
 this->disconnect();
 return NULL;
 }

 this->isHandlingServerData = false;
 }*/

/*
 void MessengerClient::connectUser(string userName) {
 this->closeActiveGame();
 if (userName.compare(this->currentUser->getUserName()) == 0) {
 cout << "unable to create session with yourself silly :)" << endl;
 return;
 }
 this->isHandlingServerData = true;

 //this->connectingUserName = userName;
 TCPMessengerProtocol::sendCommand(this->serverSocket, START_GAME);
 TCPMessengerProtocol::sendData(this->serverSocket, userName);
 //now we wait for the server to answer with the user ip address
 int command = TCPMessanger::readCommand(this->serverSocket);
 if (command == GAME_REQUEST_ACCEPTED) {
 int listeningPort = TCPMessanger::readInt(this->serverSocket);
 this->currentUser->setListeningPort(listeningPort);
 User* targetUser = this->readUser();
 cout << "open session with user: " << userName << " succeeded" << endl;
 this->game->RunGame(this->getUserIPByUserName(userName));
 } else {
 cout << "open session with user: " << userName << " failed" << endl;
 }
 this->connectingUserName = "";
 this->isHandlingServerData = false;
 }*/
/*
 void MessengerClient::enterRoom(string roomName){
 this->closeActiveGame();
 this->connectingRoomName = roomName;
 TCPMessanger::sendCommand(this->serverSocket, ENTER_ROOM);
 TCPMessanger::sendData(this->serverSocket, roomName);
 //now we wait for the server to answer with the room access success
 this->isHandlingServerData = true;
 int command = TCPMessanger::readCommand(this->serverSocket);
 if(command == JOINED_ROOM){
 User* user = this->readUser();
 this->currentUser->setListeningPo9999rt(user->getListeningPort());
 delete user;

 cout << "receiving room users.." << endl;
 vector<User*>* roomUsers = this->getRoomUsers(roomName);
 Room* targetRoom = new Room(roomName, roomUsers);

 cout << "entering room: " << roomName << " succeeded" << endl;
 this->udpMessenger = new UDPMessenger(this->currentUser, targetRoom);
 }
 else{
 cout << "entering room: " << roomName << " failed" << endl;
 }
 this->connectingRoomName = "";
 this->isHandlingServerData = false;
 }

 vector<User*>* MessengerClient::getRoomUsers(string roomName){
 this->isHandlingServerData = true;

 vector<User*>* users = new vector<User*>();
 TCPMessanger::sendCommand(this->serverSocket, GET_USERS_IN_ROOM);
 TCPMessanger::sendData(this->serverSocket, roomName);
 // now we wait for the server to answer with the room's users list
 int command = TCPMessanger::readCommand(this->serverSocket);
 if(command == GET_USERS_IN_ROOM){
 int usersLength = TCPMessanger::readInt(this->serverSocket);
 for(int i = 0; i< usersLength; i++){
 users->push_back(this->readUser());
 }
 }
 else{
 perror("Failed to get room's users list.. disconnecting from server");
 this->disconnect();
 }

 this->isHandlingServerData = false;

 return users;
 }
 *//*
 string MessengerClient::get_OpponentName() {
 return this->opponentName;
 }
 void MessengerClient::closeActiveGame(bool remote) {
 this->connectingRoomName = "";
 this->connectingUserName = "";

 if (this->game->messenger != NULL) {
 if (remote) {
 cout << "session closed by remote user" << endl;
 }
 if (this->game->messenger->isConnectedToUser() == true) {

 TCPMessanger::sendCommand(this->serverSocket, GAME_ENDED);
 TCPMessanger::sendData(this->serverSocket, opponentName);
 } else {
 string userName = this->opponentName;
 cout << "closing session with: " << userName << endl;
 }
 this->game->messenger->close();
 delete this->game;
 this->game = NULL;
 cout << "active session closed" << endl;
 }
 }
 *//*
 void MessengerClient::run() {
 this->isRunning = true;
 while (this->isRunning) {
 if (this->isHandlingServerData) {
 sleep(1);
 continue;
 }
 MultipleTCPSocketsListener* listener = new MultipleTCPSocketsListener();

 listener->addSocket(this->serverSocket);
 TCPSocket* readySocket = listener->listenToSocket();
 listener->close();
 delete listener;
 if (readySocket == NULL || this->isHandlingServerData) {
 continue;
 }
 handle();
 }
 }*/
