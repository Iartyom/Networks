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
	if (!this->isActiveSession()) {
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
	if (/*!validateConnectedServer() ||*/ !validateLoggedIn(true))
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
			if (this->isActiveSession()) {
				status += (", in active session "
						+ this->opponent->getUserName());

			}
		}
	}
}

void MessengerClient::registerUser(string userName, string password) {

	TCPMessengerProtocol::sendCommand(this->serverSocket, REGISTER);
	TCPMessengerProtocol::sendData(this->serverSocket, userName);
	TCPMessengerProtocol::sendData(this->serverSocket, password);
	this->isHandlingServerData = true;
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
	cout<< "connected!!"<<endl;
	this->start();
}
void MessengerClient::login(string userName, string password) {
	this->isHandlingServerData = true;
	TCPMessengerProtocol::sendCommand(this->serverSocket, LOGIN);
	TCPMessengerProtocol::sendData(this->serverSocket, userName);
	TCPMessengerProtocol::sendData(this->serverSocket, password);
	//this->isHandlingServerData = true;
	//now we wait for the server to answer if the login succeeded
	cout << "here 1 "<<this->serverSocket->socket()<<endl;

	int command = TCPMessengerProtocol::readCommand(this->serverSocket);
	cout << "here 2"<<endl;
	if (command == LOGIN) {
		this->currentUser = new User(userName); // the ip and port is not relevant for our user..
		cout << "Login succeeded, you are now logged as: " << userName << " :) "
				<< endl;
	} else {
		cout << "Login failed, please try again, your user name or password are incorrect" << endl;
	}
	this->isHandlingServerData = false;
}


bool MessengerClient::isLoggedIn() {
	return this->currentUser != NULL;
}

bool MessengerClient::isConnected() {
	return this->serverSocket != NULL && this->serverSocket->socket() > 0;
}

bool MessengerClient::isActiveSession() {
	return this->game != NULL;
}

User* MessengerClient::getCurrentUser() {
	return this->currentUser;
}

void MessengerClient::printConnectedUsers() {
	this->isHandlingServerData = true;

	cout << "Connected Users: " << endl;
//	vector<User*> users;
	TCPMessengerProtocol::sendCommand(this->serverSocket, GET_ONLINE_USERS_LIST);
	// now we wait for the server to answer with the connected users list
	int command = TCPMessengerProtocol::readCommand(this->serverSocket);
	cout<< command<<endl;
	if (command == GET_ONLINE_USERS_LIST) {
		int usersLength = TCPMessengerProtocol::readInt(this->serverSocket);
		for (int i = 0; i < usersLength; i++) {
			cout << TCPMessengerProtocol::readData(this->serverSocket);
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
	TCPMessengerProtocol::sendCommand(this->serverSocket,
	REQUEST_TO_START_GAME);
	TCPMessengerProtocol::sendData(this->serverSocket, user_target);
	//now we wait for the server to answer with game approval
	this->isHandlingServerData = true;
	int command = TCPMessengerProtocol::readCommand(this->serverSocket);
	if (command == GAME_REQUEST_ACCEPTED) {
		cout << "receiving opponent details." << endl;
		opponent = MessengerClient::readUser(this->serverSocket);
//starting the game aageinst opponents IP
		game->RunGame(opponent->getIP());

//if they refuse or we have a problem
	} else if (command == SESSION_REFUSED) {
		cout
				<< "receiving opponent refusal , i think he hates you, or he's just busy"
				<< endl;
	} else {
		cout << "starting game with:" << opponent->getUserName()
				<< " failed for unknowen reason DAMN" << endl;
	}
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
	this->closeActiveSession(true);

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

}
void MessengerClient::closeActiveSession(bool remote) {
	if (this->game != NULL) {
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

		handle();
	}
}
void MessengerClient::handle() {
	int command = TCPMessengerProtocol::readCommand(this->serverSocket);
	string userName;
	//User* user;
	int listeningPort;
	//commands to execute

	switch (command) {

	case REQUEST_TO_START_GAME: {
		cout << "received open game request from: ";
		listeningPort = TCPMessengerProtocol::readInt(this->serverSocket);
		string potencialOpponent = TCPMessengerProtocol::readData(
				this->serverSocket);
		//	user = this->readUser();
		cout << potencialOpponent << endl;
		cout << "write 'yes' to accept or 'no' to decline";
		string awnser;
		cin >> awnser;
		if (awnser == "yes") {
			this->closeActiveSession();
			TCPMessengerProtocol::sendCommand(this->serverSocket,
			GAME_REQUEST_ACCEPTED);
//			this->currentUser->setListeningPort(listeningPort);
			this->isHandlingServerData = true;
			int command = TCPMessengerProtocol::readCommand(this->serverSocket);
			if (command == START_GAME) {
				opponent = MessengerClient::readUser(this->serverSocket);
				cout << "opening game by remote request with: "
						<< opponent->getUserName() << endl;
				this->isHandlingServerData = false;
				game->RunGame(opponent->getIP());
			}
		} else {
			TCPMessengerProtocol::sendCommand(this->serverSocket,
			GAME_REQUEST_REJECTED);
			cout << "you rejected the genres offer";
		}
	}
		break;
	case GAME_CANCEL: {
		this->closeActiveSession(true);
	}
		break;
		/*
		 case EXIT:
		 this->closeActiveSession(true);
		 break;
		 case JOINED_ROOM:
		 user = this->readUser();
		 if (this->udpMessenger == NULL) {
		 return;
		 }
		 this->udpMessenger->addUserToRoom(user);
		 break;
		 case LEFT_ROOM:
		 userName = TCPMessanger::readData(this->serverSocket);
		 if (this->udpMessenger == NULL) {
		 return;
		 }
		 this->udpMessenger->removeUserFromRoom(userName);
		 break;*/
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

/*
 Room* MessengerClient::readRoom(){
 string roomName = TCPMessanger::readData(this->serverSocket);
 return new Room(roomName);
 }
 */
void MessengerClient::close() {
	this->isRunning = false;
	this->waitForThread();
}

MessengerClient::~MessengerClient() {
}
}
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
 this->closeActiveSession();
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
 this->closeActiveSession();
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
 void MessengerClient::closeActiveSession(bool remote) {
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
