
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "RakNet/RakPeerInterface.h"
#include "Raknet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"  // MessageID
#include "Battleship.h"
#include "TicTacToe.h"
#include "Input.h"

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_SEND_MESSAGE = ID_USER_PACKET_ENUM + 2,
	ID_RECIEVE_MESSAGE = ID_USER_PACKET_ENUM + 3,
	ID_BROADCAST_USER = ID_USER_PACKET_ENUM + 4,
	ID_RECEIVE_TIC_TAC_TOE = ID_USER_PACKET_ENUM + 5,
	ID_RECEIVE_BATTLESHIP = ID_USER_PACKET_ENUM + 6

};

using namespace RakNet;
using namespace std;

unsigned int maxClients = 2;
unsigned short serverPort = 6000;

bool canTypeMessage = true;
bool sendUserMessage = false;

const int maxCharInMessage = 250;
const int maxCharInIP = 19;
const int maxCharInName = 12;
const int maxUsers = 2; // make sure this is same as max Clients

unsigned int currentClients = 0;
bool continueLoop = true;

bool printClientsNames = false;
bool isServer;
bool privateMessage = false;

#pragma pack(push, 1)
struct Participant
{
	unsigned char typeId; // Your type here
	// Your data here
	char name[maxCharInName];
	char message[maxCharInMessage];
	char nameOfMessageRecipient[maxCharInMessage];

	void AddToParticipantMsg(char newChar)
	{
		for (int i = 0; i < maxCharInMessage; i++)
		{
			if (message[i] == -52)
			{
				message[i] = newChar;
				break;
			}
		}
	}

	void DeleteParticipantLastCharOfMsg()
	{
		for (int i = 0; i < maxCharInMessage; i++)
		{
			if (message[i] == NULL && i != 0)
			{
				message[i - 1] = NULL;
				break;
			}
			else if (i == maxCharInMessage - 1)
			{
				message[maxCharInMessage - 1] = NULL;
				break;
			}
		}
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Host
{
	unsigned char typeId; // Your type here
	// Your data here
	char userName[maxCharInName];
	char listOfParticipantsName[maxUsers][maxCharInName];
	char participantsName[maxCharInName];
	char messageText[maxCharInMessage];
	bool privateMsg = false;

	void AddToHostMsg(char newChar)
	{
		for (int i = 0; i < maxCharInMessage; i++)
		{
			if (messageText[i] == NULL || messageText[i] == -52)
			{
				messageText[i] = newChar;
				break;
			}
		}
	}

	void DeleteHostLastCharOfMsg()
	{
		for (int i = 0; i < maxCharInMessage; i++)
		{
			if (messageText[i] == NULL && i != 0)
			{
				messageText[i - 1] = NULL;
				break;
			}
			else if (i == maxCharInMessage - 1)
			{
				messageText[maxCharInMessage - 1] = NULL;
				break;
			}
		}
	}
};
#pragma pack(pop)

struct TicTacToeFullGameData
{
	unsigned char typeId = ID_RECEIVE_TIC_TAC_TOE;
	char gameData[3][3];
};

struct BattleShipFullGameData
{
	unsigned char typeId = ID_RECEIVE_BATTLESHIP;
	char gameData[10][10];
};

struct BattleshipData
{
	unsigned char typeId = ID_RECEIVE_BATTLESHIP;
	char yPos; // has to be a letter
	char xPos; // has to be a Number
};

struct TicTacToeData
{
	unsigned char typeId = ID_RECEIVE_TIC_TAC_TOE;
	char ticTacToePos; // would be a... number?
};


// participant sends a msg
// host gets it...
// if it is not addressed to the host they send it to the right person (for private)
// the recipient gets a cout of the msg
// if the message is a broadcast the message is sent to everyone

// type msg>>press enter>>send package with the id of private or broadcast message>>host gets package>>either prints to screen (if msg to host)>>
// send msg to other person with msg ID Message receive

void DoMyPacketHandlerHost(Host* hostPack);
void GetInput(char msg[]);

void RecieveClientInfo(Packet* packet, Host& myHost)
{
	// Cast the data to the appropriate type of struct
	Participant* s = (Participant*)packet->data;
	//	assert(packet->length == sizeof(Client)); // This is a good idea if you’re transmitting structs.
	if (sizeof(packet->data) != sizeof(Participant*))
	{
		return;
	}
	else
	{
		for (int i = 0; i < maxCharInName; i++)
		{
			if (s->name[i] == '\n')
			{
				break;
			}
			else
			{
				myHost.listOfParticipantsName[currentClients][i] = s->name[i];
				std::cout << myHost.listOfParticipantsName[currentClients][i];

			}
		}

		currentClients++;
	}

	// Perform the functionality for this type of packet, with your struct,  MyStruct *s
}

void PrintClientNames(Host& myHost, RakNet::SystemAddress listOfAddress[])
{
	if (currentClients > 0)
	{
		for (unsigned int k = 0; k < currentClients; k++)
		{

			for (unsigned int j = 0; j < maxCharInName; j++)
			{
				if (currentClients > 0 && myHost.listOfParticipantsName[k][j] != -52)
				{
					std::cout << myHost.listOfParticipantsName[k][j];
				}


			}

			if (currentClients > 0)
			{
				std::cout << listOfAddress[k].ToString();
			}
			printf("\n");
		}
	}
	else
	{
		std::cout << "No clients" << endl;
	}
	printf("\n");
	printClientsNames = false;
}

void DisplayConsoleWindow()
{
	printf("\033[36m");
	printf("-------------------------------------Console Window-------------------------------------\n");
	printf("\033[35m");
	printf("-----ESC: Leave Chat   PAGEUP: Send private message   PAGEDOWN: Send public message-----\n");
	printf("------------------------------TAB: Open Console Window----------------------------------\n");
	printf("\033[0m");
}

void DisplayHostWindow()
{
	printf("\033[31m");
	printf("-------------------------Admin Console Window-------------------------------\n");
	printf("\033[32m");
	printf("----------------ESC: Close Program   TAB: Open Console window---------------\n");
	printf("-------------------CTRL: Print user names and IP addresses------------------\n");
	printf("\033[0m");
}

// so instead of sending host information we should create a package of game data
// for both the battleship and the tic tac toe we will send coordinate data 
//   1  2  3
// a
// b
// c
//--------------------------------------------------------------------------------

// let the code decide who wins and loses then send a message to both about the winning
// the spaces will display x and o in the spots

// -------------------------------------------------------------------------------

// tic tac toe wont need the coodinates we could just get the number 1-9
//
// 1 2 3 
// 4 5 6 
// 7 8 9 
//
//---------------------------------------------------------------------------------

// for turn based... we need to send info and wait until there is a return to do the next turn
//
// --------------------------------------------------------------------------------
//
// for battle ship the screen shows X for hits O for misses and ~ as water tiles

int main(void)
{
	RakNet::SystemAddress serverAddress;
	Input myInput = Input();
	//char listOfUsers[maxUsers][maxCharInName];
	RakNet::SystemAddress listOfParticipantAddress[maxUsers];

	char tempName[maxCharInName];
	string checking;
	printf("Please enter you name (must be within 12 Characters):\n");
	//std::cin >> checking;
	fgets(tempName, 12, stdin);

	std::cout << endl;


	Host host;
	Participant participant;
	// get key state and then addd the state of teh key board to an array for messaging

	char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	//bool isServer;
	RakNet::Packet* packet;

	printf("(H)ost or (J)oin a Server?\nPlease Enter \"h\" to Host or \"j\" to Join\n");
	fgets(str, 512, stdin);
	if ((str[0] == 'j') || (str[0] == 'J'))
	{
		SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;
		for (int i = 0; i < maxCharInName; i++)
		{
			participant.name[i] = tempName[i];
		}
	}
	else {
		SocketDescriptor sd(serverPort, 0);
		peer->Startup(maxClients, &sd, 1);
		isServer = true;

		for (int i = 0; i < maxCharInName; i++)
		{
			participant.name[i] = tempName[i];
		}
	}

	if (isServer)
	{
		printf("Starting the chat room for ");
		printf(&participant.name[0]);

		DisplayHostWindow();

		// how do we get our IP address
		// We need to let the server accept incoming connections from the clients


		//std::cout << "\nserver address:" << serverAddress.ToString() << std::endl;

		peer->SetMaximumIncomingConnections(maxClients);
	}
	else {
		printf("Enter server IP or hit enter for 127.0.0.1\n");
		fgets(str, 512, stdin);
		if (str[0] == 10) {
			strcpy(str, "127.0.0.1");
		}
		printf("Connecting to the chat room for ");
		printf(&participant.name[0]);

		DisplayConsoleWindow();

		//std::cout << "\nserver address:" << serverAddress.ToString() << std::endl;

		peer->Connect(str, serverPort, 0, 0);
	}


	// TODO - Add code body here
	while (continueLoop)
	{
		Participant* tempPart;
		Host* tempHost;
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			{
				printf("Another client has disconnected.\n");
				break;
			}
			case ID_REMOTE_CONNECTION_LOST:
			{
				printf("Another client has lost the connection.\n");
				break;
			}
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
			{
				printf("Another client has connected.\n");
				break;
			}
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Our connection request has been accepted.\n");
				participant.typeId = ID_BROADCAST_USER;
				serverAddress = packet->systemAddress;
				peer->Send((const char*)&participant, sizeof(participant), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

				break;
			}
			case ID_NEW_INCOMING_CONNECTION:
			{
				printf("A connection is incoming.\n");

				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The chat room is full.\n");
				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
			{
				if (isServer) {
					printf("A client has disconnected.\n");
				}
				else {
					printf("We have been disconnected from the host.\n");
				}
				break;
			}
			case ID_CONNECTION_LOST:
			{
				if (isServer) {
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection to the room has been lost.\n");
				}
				break;
			}
			case ID_SEND_MESSAGE:
			{
				// will only go to the host which then will send a host package with the info
				// info data will be ID_RECIEVE MESSAGE

				printf("\nHost Received Message To Send\n");

				// make the id to send to the recipient
				host.typeId = ID_RECIEVE_MESSAGE;

				Participant* temp = (Participant*)packet->data;
				std::string recipientName;
				std::string recipientStringAddress[maxCharInIP];
				RakNet::SystemAddress recipientAddress;
				bool userExists = false;
				bool msgToHost = false;


				// ge the message and get the name from the message... 

				// transfer the name of participant to the host
				for (int i = 0; i < maxCharInName; i++)
				{
					host.participantsName[i] = temp->name[i];
				}


				// transfer the name of the recipent to the host (find it from the beginning of the message)
				for (int i = 0; i < maxCharInName; i++)
				{
					if (temp->message[i] != '/n' && temp->message[i] != ' ' && temp->message[i] != -52)
					{
						recipientName.push_back(temp->message[i]);
					}
					else
					{
						break;
					}
				}

				//if ALL or all then broadcast to every
				if (recipientName == "ALL" || recipientName == "all")
				{
					//loop through all addresses and send through that
					for (unsigned int i = 0; i < currentClients; i++)
					{
						peer->Send((const char*)&host, sizeof(host), HIGH_PRIORITY, RELIABLE_ORDERED, 0, listOfParticipantAddress[i], false);
					}
				}
				else
				{
					// check for the recipient address 
					for (unsigned int i = 0; i < currentClients; i++)
					{
						for (int j = 0; j < recipientName.size(); j++)
						{
							if (recipientName[j] == host.listOfParticipantsName[i][j])
							{
								if (j == recipientName.size() - 1)
								{
									userExists = true;


									recipientAddress = listOfParticipantAddress[i];
									break;
								}
							}
							else
							{
								break;
							}
						}
					}

					for (int j = 0; j < recipientName.size(); j++)
					{
						if (recipientName[j] == participant.name[j])
						{
							if (j == recipientName.size() - 1)
							{
								for (size_t i = recipientName.size(); i < maxCharInMessage; i++)
								{
									if (temp->message[i] != -52 || i == maxCharInMessage)
									{
										std::cout << temp->message[i];
									}
									else
									{
										msgToHost = true;
										break;
									}
								}

								break;
							}
						}
						else
						{
							break;
						}
					}

					for (int i = 0; i < maxCharInMessage; i++)
					{
						host.messageText[i] = temp->message[i];
					}

					if (msgToHost)
					{
						break;
					}
					else
					{
						if (userExists)
						{
							//send message to corresponging recipient  system address

							peer->Send((const char*)&host, sizeof(host), HIGH_PRIORITY, RELIABLE_ORDERED, 0, recipientAddress, false);

						}
						else
						{
							// make the message say that the person does not exist 
							//resets teh host msg
							for (int i = 0; i < maxCharInMessage; i++)
							{
								host.messageText[i] = -52;
							}
							// message that they do not exist
							{
								host.AddToHostMsg('T');
								host.AddToHostMsg('H');
								host.AddToHostMsg('E');
								host.AddToHostMsg(' ');
								host.AddToHostMsg('U');
								host.AddToHostMsg('S');
								host.AddToHostMsg('E');
								host.AddToHostMsg('R');
								host.AddToHostMsg(' ');
								host.AddToHostMsg('D');
								host.AddToHostMsg('O');
								host.AddToHostMsg('E');
								host.AddToHostMsg('S');
								host.AddToHostMsg(' ');
								host.AddToHostMsg('N');
								host.AddToHostMsg('O');
								host.AddToHostMsg('T');
								host.AddToHostMsg(' ');
								host.AddToHostMsg('E');
								host.AddToHostMsg('X');
								host.AddToHostMsg('I');
								host.AddToHostMsg('S');
								host.AddToHostMsg('T');
							}
							// send that the user does not exist
							peer->Send((const char*)&host, sizeof(host), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						}
					}

				}


				//resets teh host msg
				for (int i = 0; i < maxCharInMessage; i++)
				{
					host.messageText[i] = -52;
				}

				break;
			}
			case ID_RECIEVE_MESSAGE:
			{
				// receives the server's msg and prints to screen
				printf("\n incoming message: \n");
				tempHost = (Host*)packet->data;
				std::cout << "\nIncoming message from ";
				for (int i = 0; i < maxCharInName; i++)
				{
					if (tempHost->participantsName[i] != NULL && tempHost->participantsName[i] != -52 && tempHost->participantsName[i] != '\n')
					{
						std::cout << tempHost->participantsName[i];
					}
				}
				std::cout << ": \n";
				DoMyPacketHandlerHost(tempHost);
				break;
			}
			case ID_BROADCAST_USER:
			{
				printf("Broadcasting new user!!!\n");

				listOfParticipantAddress[currentClients] = packet->systemAddress;

				RecieveClientInfo(packet, host);
				
				host.typeId = ID_RECIEVE_MESSAGE;


				// this is message to say that the user has entered the room
				{
					for (int i = 0; i < maxCharInName; i++)
					{
						Participant* temp = (Participant*)packet->data;

						if (temp->name[i] != -52 && temp->name[i] != '\n')
						{
							host.messageText[i] = temp->name[i];
						}
						else
						{
							break;
						}
					}
					host.AddToHostMsg(' ');

					host.AddToHostMsg('h');
					host.AddToHostMsg('a');
					host.AddToHostMsg('s');
					host.AddToHostMsg(' ');
					host.AddToHostMsg('j');
					host.AddToHostMsg('o');
					host.AddToHostMsg('i');
					host.AddToHostMsg('n');
					host.AddToHostMsg('e');
					host.AddToHostMsg('d');

					host.participantsName[0] = 'a';
					host.participantsName[1] = 'd';
					host.participantsName[2] = 'm';
					host.participantsName[3] = 'i';
					host.participantsName[4] = 'n';
				}


				//std::cout << "packet address" << packet->systemAddress.ToString() << std::endl;
				for (int i = 0; i < currentClients; i++)
				{
					peer->Send((const char*)&host, sizeof(host), HIGH_PRIORITY, RELIABLE_ORDERED, 0, listOfParticipantAddress[i], false);
				}

				for (int i = 0; i < maxCharInMessage; i++)
				{
					host.messageText[i] = -52;
				}
			}
			break;
			default:
			{
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}

			}
		}

		if (printClientsNames)
		{
			PrintClientNames(host, listOfParticipantAddress);
		}


		if (sendUserMessage)
		{
			sendUserMessage = false;
			// will send the msg
			// will send the name of the person from
			// will need to send who to as well
			// the ID will either be broadcast or private
			// testing:


			cout << "\nSending message: \n";

			for (int i = 0; i < maxCharInMessage; i++)
			{
				if (participant.message[i] == -52)
				{
					break;
				}
				else
				{
					cout << participant.message[i];
				}
			}
			cout << "\n";
			participant.typeId = ID_SEND_MESSAGE;

			//not working prob because of the server address

			std::cout << "\nserverAddress: " << serverAddress.ToString() << std::endl;
			peer->Send((const char*)&participant, sizeof(participant), HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);



			for (int i = 0; i < maxCharInMessage; i++)
			{
				participant.message[i] = -52;
			}
		}

		GetInput(participant.message);

	}

	printf("im done lol");

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}

void GetInput(char tempMsg[])
{
	//std::cout<<GetAsyncKeyState('A');

	int j = 0;
	bool pressingKey = false;

	for (char key = ' '; key <= '~'; key++)
	{
		if (GetAsyncKeyState(key) != 0)
		{
			//			std::cout << "we are pressing button" << std::endl;
			if (canTypeMessage)
			{
				for (int i = 0; i < maxCharInMessage; i++)
				{
					if (tempMsg[i] == -52 || tempMsg[i] == NULL)
					{
						//std::cout << key;
						canTypeMessage = false;
						tempMsg[i] = key;
						std::cout << tempMsg[i];
						j++;
						break;
					}

				}
			}
			pressingKey = true;


			break;
		}
	}
	if (GetAsyncKeyState(VK_BACK) != 0)
	{
		if (canTypeMessage)
		{
			//std::cout << "backspace";
			for (int i = 1; i < maxCharInMessage; i++)
			{
				if (tempMsg[i] == NULL || tempMsg[i] == -52)
				{
					tempMsg[i - 1] = -52;
				}
			}
			j--;
			tempMsg[j] = -52;

			std::cout << "\b" << " " << "\b";
		}
		pressingKey = true;

	}
	if (GetAsyncKeyState(VK_ESCAPE) != 0)
	{
		if (canTypeMessage)
		{
			//std::cout << "backspace";
			continueLoop = false;
		}
		pressingKey = true;

	}
	if (GetAsyncKeyState(VK_CONTROL) != 0)
	{
		if (isServer)
		{
			if (canTypeMessage)
			{

				printClientsNames = true;
			}
			pressingKey = true;
		}


	}
	if (GetAsyncKeyState(VK_TAB) != 0)
	{
		if (canTypeMessage)
		{
			//std::cout << "backspace";
			if (isServer)
			{
				DisplayHostWindow();
			}
			else
			{
				DisplayConsoleWindow();
			}
		}
		pressingKey = true;

	}
	if (GetAsyncKeyState(VK_PRIOR) != 0)
	{
		if (!isServer)
		{
			if (canTypeMessage)
			{
				//std::cout << "backspace";
				printf("\nsend private message\n");
				privateMessage = true;
			}
			pressingKey = true;
		}

	}
	if (GetAsyncKeyState(VK_NEXT) != 0)
	{
		if (!isServer)
		{
			if (canTypeMessage)
			{
				//std::cout << "backspace";
				printf("\nsend public message\n");
				privateMessage = false;

			}
			pressingKey = true;
		}
	}
	if (GetAsyncKeyState(VK_RETURN) != 0 && tempMsg[0] != -52)
	{
		if (canTypeMessage)
		{
			// send the message
			sendUserMessage = true;
			//return true;
		}
		pressingKey = true;
	}
	else
	{
		sendUserMessage = false;
	}
	canTypeMessage = !pressingKey;


}

void DoMyPacketHandlerHost(Host* hostPack)
{
	// Cast the data to the appropriate type of struct

	for (int i = 0; i < maxCharInMessage; i++)
	{
		if (hostPack->messageText[i] == -52)
		{
			break;
		}
		else
		{
			std::cout << hostPack->messageText[i];
		}
	}


	// Perform the functionality for this type of packet, with your struct,  MyStruct *s
}

void PrintBattleshipGameData(BattleShipFullGameData bsData)
{
	// 1 2 3 4 5 6 7 8 (the top of the battle ship game
	std::cout << " ";
	for (int i = 0; i < 9; i++)
	{
		std::cout << i << " ";
	}
	std::cout << std::endl;
	//

	for (int i = 0, char side = 'a'; i < 9; i++, side++)
	{
		// a b c d e f g h (side) (the start of teh line that is)
		std::cout << side << " ";

		// orint the rest of the line data
		for (int j = 0; j < 9; j++)
		{
			std::cout << bsData.gameData[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void PrintTicTacToeGameData(TicTacToeFullGameData tttData)
{
	for (int i = 0; i < 3; i++)
	{
		std::cout << " ";
		for (int j = 0; j < 3; j++)
		{
			std::cout << tttData.gameData[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

// will return X, O or... i think -52 when using this X is for p1 O for p2 -52 for no input
char CheckTicTacToeWinCondition(TicTacToeFullGameData tttData)
{
	char winner;

	// any of the rows is same
	for (int i = 0; i <= 2; i++)
	{
		if (tttData.gameData[i][0] == tttData.gameData[i][1] && tttData.gameData[i][1] == tttData.gameData[i][2] && tttData.gameData[i][0] != 0)
		{
			winner = tttData.gameData[i][0];
			return winner;
		}
	}

	// any of the columns is same
	for (int i = 0; i <= 2; i++)
	{
		if (tttData.gameData[0][i] == tttData.gameData[1][i] && tttData.gameData[1][i] == tttData.gameData[2][i] && tttData.gameData[0][i] != 0)
		{
			winner = tttData.gameData[0][i];
			return winner;
		}
	}

	// 1st diagonal is same
	if (tttData.gameData[0][0] == tttData.gameData[1][1] && tttData.gameData[1][1] == tttData.gameData[2][2] && tttData.gameData[0][0] != 0)
	{
		winner = tttData.gameData[0][0];
		return winner;
	}

	// 2nd diagonal is same
	if (tttData.gameData[0][2] == tttData.gameData[1][1] && tttData.gameData[1][1] == tttData.gameData[2][0] && tttData.gameData[0][2] != 0)
	{
		winner = tttData.gameData[0][2];
		return winner;
	}

	// if we reached here nobody has won yet

	// if any empty box on tttData.gameData then play on
	for (int i = 0; i <= 2; i++)
	{
		for (int j = 0; j <= 2; j++)
		{
			if (tttData.gameData[i][j] == 0)
			{
				winner = 0;
				return winner;
			}
		}
	}
}