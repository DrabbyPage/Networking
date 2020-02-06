
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "RakNet/RakPeerInterface.h"
#include "Raknet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"  // MessageID
#include "GameMessageEnums.h"	//this is where all the game messages are
#include "Battleship.h"
#include "TicTacToe.h"
#include "Input.h"

using namespace RakNet;
using namespace std;

unsigned int maxClients = 4;
unsigned short serverPort = 6000;

//bool canTypeMessage = true;
//bool sendUserMessage = false;

bool playBattleship;
bool playTicTacToe;

bool isSpectator;
bool isClient;

bool turnDone = false;

const int maxCharInMessage = 250;
const int maxCharInIP = 19;
const int maxCharInName = 12;
const int maxUsers = 4; // make sure this is same as max Clients

unsigned int currentClients = 0;
bool continueLoop = true;

//bool printClientsNames = false;
bool isServer;
//bool privateMessage = false;

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


// participant sends a msg
// host gets it...
// if it is not addressed to the host they send it to the right person (for private)
// the recipient gets a cout of the msg
// if the message is a broadcast the message is sent to everyone

// type msg>>press enter>>send package with the id of private or broadcast message>>host gets package>>either prints to screen (if msg to host)>>
// send msg to other person with msg ID Message receive

void DoMyPacketHandlerHost(Host* hostPack);
void PrintBattleshipGameData(BattleShipFullGameData bsData);

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
			std::printf("\n");
		}
	}
	else
	{
		std::cout << "No clients" << endl;
	}
	std::printf("\n");
	//printClientsNames = false;

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
// for battle ship the screen shows X for hits O for misses and ~ as water tiles and # for a ship

int main(void)
{
	RakNet::SystemAddress serverAddress;
	Input myInput;
	//char listOfUsers[maxUsers][maxCharInName];
	RakNet::SystemAddress listOfParticipantAddress[maxUsers];

	Host host;
	Participant participant;

	//BattleshipManager battleshipManager;
	//TicTacToeFullGameData ticTacToeManager;

	TicTacToe ticTacToeManager;

	char tempName[maxCharInName];
	string checking;
	std::printf("Please enter you name (must be within 12 Characters):\n");
	//std::cin >> checking;
	fgets(tempName, 12, stdin);

	std::cout << std::endl;

	// get key state and then addd the state of teh key board to an array for messaging

	char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	//bool isServer;
	RakNet::Packet* packet;

	// asking if going to be a server or join another
	std::printf("(H)ost or (J)oin a Server?\nPlease Enter \"h\" to Host, \"j\" to Join, \"s\" to spectate\n");
	fgets(str, 512, stdin);
	if ((str[0] == 'j') || (str[0] == 'J'))
	{
		SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;
		myInput.setIsServer(isServer);
		isClient = true;
		for (int i = 0; i < maxCharInName; i++)
		{
			participant.name[i] = tempName[i];
		}
	}
	else if ((str[0] == 'h') || (str[0] == 'H')) {
		SocketDescriptor sd(serverPort, 0);
		peer->Startup(maxClients, &sd, 1);
		isServer = true;
		myInput.setIsServer(isServer);
		for (int i = 0; i < maxCharInName; i++)
		{
			participant.name[i] = tempName[i];
		}
	}
	else
	{
		//spectator stuff
		isSpectator = true;
		SocketDescriptor sd(serverPort, 0);
		peer->Startup(maxClients, &sd, 1);
		isServer = false;
		myInput.setIsServer(isServer);
		for (int i = 0; i < maxCharInName; i++)
		{
			participant.name[i] = tempName[i];
		}
	}

	// starting server and joining teh server through input
	if (isServer)
	{
		std::printf("You are a host!\n");
		std::printf("Starting the chat room for ");
		std::printf(&participant.name[0]);
		//myInput.setIsServer(isServer);
		myInput.DisplayHostWindow();

		peer->SetMaximumIncomingConnections(maxClients);
	}
	else if (isClient)
	{
		std::printf("You are a player!\n");
		std::printf("Enter server IP or hit enter for 127.0.0.1\n");
		fgets(str, 512, stdin);
		if (str[0] == 10) {
			strcpy(str, "127.0.0.1");
		}
		std::printf("Connecting to the chat room for ");
		std::printf(&participant.name[0]);
		//myInput.setIsServer(isServer);
		myInput.DisplayConsoleWindow();

		peer->Connect(str, serverPort, 0, 0);
	}
	else
	{
		std::printf("You are a spectator!\n");
		std::printf("Enter server IP or hit enter for 127.0.0.1\n");
		fgets(str, 512, stdin);
		if (str[0] == 10) {
			strcpy(str, "127.0.0.1");
		}
		std::printf("Connecting to the chat room for ");
		std::printf(&participant.name[0]);
		//myInput.setIsServer(isServer);
		myInput.DisplayConsoleWindow();

		peer->Connect(str, serverPort, 0, 0);
	}


	// TODO - Add code body here
	while (continueLoop)
	{
		Host* tempHost;
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			{
				std::printf("Another client has disconnected.\n");
				break;
			}
			case ID_REMOTE_CONNECTION_LOST:
			{
				std::printf("Another client has lost the connection.\n");
				break;
			}
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
			{
				std::printf("Another client has connected.\n");
				break;
			}
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				std::printf("Our connection request has been accepted.\n");
				participant.typeId = ID_BROADCAST_USER;
				serverAddress = packet->systemAddress;
				peer->Send((const char*)&participant, sizeof(participant), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

				break;
			}
			case ID_NEW_INCOMING_CONNECTION:
			{
				std::printf("A connection is incoming.\n");

				break;
			}
			case ID_NO_FREE_INCOMING_CONNECTIONS:
			{
				std::printf("The chat room is full.\n");
				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
			{
				if (isServer) {
					std::printf("A client has disconnected.\n");
				}
				else {
					std::printf("We have been disconnected from the host.\n");
				}
				break;
			}
			case ID_CONNECTION_LOST:
			{
				if (isServer) {
					std::printf("A client lost the connection.\n");
				}
				else {
					std::printf("Connection to the room has been lost.\n");
				}
				break;
			}
			case ID_SEND_MESSAGE:
			{
				// will only go to the host which then will send a host package with the info
				// info data will be ID_RECIEVE MESSAGE

				std::printf("\nHost Received Message To Send\n");

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
					if (temp->message[i] != '\n' && temp->message[i] != ' ' && temp->message[i] != -52)
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
				std::printf("\n incoming message: \n");
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
				std::printf("Broadcasting new user!!!\n");

				listOfParticipantAddress[currentClients] = packet->systemAddress;

				RecieveClientInfo(packet, host);

				


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
					host.AddToHostMsg('\n');

					host.participantsName[0] = 'a';
					host.participantsName[1] = 'd';
					host.participantsName[2] = 'm';
					host.participantsName[3] = 'i';
					host.participantsName[4] = 'n';
				}

					host.typeId = ID_SET_GAME_TYPE;
					peer->Send((const char*)&host, sizeof(host), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				
				host.typeId = ID_RECIEVE_MESSAGE;
				//std::cout << "packet address" << packet->systemAddress.ToString() << std::endl;
				for (unsigned int i = 0; i < currentClients; i++)
				{
					peer->Send((const char*)&host, sizeof(host), HIGH_PRIORITY, RELIABLE_ORDERED, 0, listOfParticipantAddress[i], false);
				}

				for (int i = 0; i < maxCharInMessage; i++)
				{
					host.messageText[i] = -52;
				}

				//if we are a client, we want to go to play 


			}
			break;
			case ID_RECEIVE_BATTLESHIP:
			{
				break;
			}
			case ID_RECEIVE_BATTLESHIP_FULL:
			{
				break;
			}
			case ID_RECEIVE_TIC_TAC_TOE:
			{
				break;
			}
			case ID_RECEIVE_TICTACTOE_FULL:
			{
				break;
			}
			case ID_SET_GAME_TYPE:
			{
				std::printf("Please enter what game you would like to play: \"t\" for TicTacToe, \"b\" for BattleShip \n");
				fgets(str, 512, stdin);
				if ((str[0] == 't') || (str[0] == 'T'))
				{
					playTicTacToe = true;
					playBattleship = false;
				}
				else if ((str[0] == 'b') || (str[0] == 'B'))
				{
					playTicTacToe = false;
					playBattleship = true;
				}
				break;
			}
			default:
			{
				std::printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}

			}
		}

		if (myInput.getPrintClientsNames())
		{
			PrintClientNames(host, listOfParticipantAddress);
			myInput.setPrintClientsNames(false);
		}

		// sending the msg
		if (myInput.getSendUserMessage())
		{
			if (!isServer)
			{
				myInput.setSendUserMessage(false);
				//sendUserMessage = false;
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

			}
			else
			{
				// make the id to send to the recipient
				host.typeId = ID_RECIEVE_MESSAGE;

				std::string recipientName;
				std::string recipientStringAddress[maxCharInIP];
				RakNet::SystemAddress recipientAddress;
				bool userExists = false;
				bool msgToHost = false;


				// ge the message and get the name from the message... 

				// transfer the name of participant to the host
				for (int i = 0; i < maxCharInName; i++)
				{
					host.participantsName[i] = host.userName[i];// ->name[i];
				}


				// transfer the name of the recipent to the host (find it from the beginning of the message)
				for (int i = 0; i < maxCharInName; i++)
				{
					if (participant.message[i] != '\n' && participant.message[i] != ' ' && participant.message[i] != -52)
					{
						recipientName.push_back(participant.message[i]);
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
									if (participant.message[i] != -52 || i == maxCharInMessage)
									{
										std::cout << participant.message[i];
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
						host.messageText[i] = participant.message[i];
					}

					if (userExists)
					{
						//send message to corresponging recipient  system address

						peer->Send((const char*)&host, sizeof(host), HIGH_PRIORITY, RELIABLE_ORDERED, 0, recipientAddress, false);

					}
					else
					{
						// make the message say that the person does not exist 
						//resets teh host msg
						std::cout << "The User Does Not Exist" << std::endl;
						for (int i = 0; i < maxCharInMessage; i++)
						{
							host.messageText[i] = -52;
						}
					}

				}


				//resets teh host msg
				for (int i = 0; i < maxCharInMessage; i++)
				{
					host.messageText[i] = -52;
				}
			}


			for (int i = 0; i < maxCharInMessage; i++)
			{
				participant.message[i] = -52;
			}
		}

		if (playTicTacToe)
		{
			//do tictactoe
			if (!turnDone)
			{
				ticTacToeManager.PrintBoard();
				turnDone = true;
			}


		}
		else //battleship
		{
			//do battleship


		}


		myInput.setIsServer(isServer);
		myInput.setMaxCharInMessage(maxCharInMessage);
		continueLoop = myInput.getContinueLoop();
		myInput.GetInput(participant.message);
		//GetInput(participant.message);

	}

	std::printf("im done lol");

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
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