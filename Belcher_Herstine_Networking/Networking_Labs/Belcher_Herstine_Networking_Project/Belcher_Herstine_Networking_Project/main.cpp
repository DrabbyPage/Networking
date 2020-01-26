
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "RakNet/RakPeerInterface.h"
#include "Raknet/MessageIdentifiers.h"
#include "Raknet/BitStream.h"
#include "Raknet/RakNetTypes.h"  // MessageID

using namespace RakNet;
using namespace std;

unsigned int maxClients = 2;
unsigned short serverPort = 6000;

bool canTypeMessage = true;
bool sendUserMessage = false;

const int maxCharInMessage = 250;
const int maxCharInIP = 39;
const int maxCharInName = 12;
const int maxUsers = 2; // make sure this is same as max Clients

unsigned int currentClients = 0;
bool continueLoop = true;

#pragma pack(push, 1)
struct Client
{
	unsigned char typeId; // Your type here
	// Your data here
	char message[10] = { 'h', 'e', 'w', 'w', 'o' };
};
#pragma pack(pop)

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
	char participantName[maxUsers][maxCharInName];
	char participantIP[maxUsers][maxCharInIP];
	char messageText[maxCharInMessage];
	bool privateMsg = false;

	void AddToHostMsg(char newChar)
	{
		for (int i = 0; i < maxCharInMessage; i++)
		{
			if (messageText[i] == NULL)
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



//#define MAX_CLIENTS 10
//#define SERVER_PORT 60000

// participant sends a msg
// host gets it...
// if it is not addressed to the host they send it to the right person (for private)
// the recipient gets a cout of the msg
// if the message is a broadcast the message is sent to everyone

// type msg>>press enter>>send package with the id of private or broadcast message>>host gets package>>either prints to screen (if msg to host)>>
// send msg to other person with msg ID Message receive

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_SEND_PRIVATE_MESSAGE = ID_USER_PACKET_ENUM + 2,
	ID_SEND_PUBLIC_BROADCAST = ID_USER_PACKET_ENUM + 3,
	ID_RECIEVE_MESSAGE = ID_USER_PACKET_ENUM + 4,
	ID_BROADCAST_USER = ID_USER_PACKET_ENUM + 5

};

unsigned char GetPacketIdentifier(Packet* packet);
void DoMyPacketHandlerClient(Packet* packet);
void DoMyPacketHandlerHost(Packet* packet);
void DoMyPacketHandlerParticipant(Packet* packet);
void GetInput(char msg[]);
void CheckKeyInput(bool keyPressed, char charUsed, char msg[]);

void RecieveClientInfo(Packet* packet, Host myHost)
{
	// Cast the data to the appropriate type of struct
	Participant* s = (Participant*)packet->data;
	//	assert(packet->length == sizeof(Client)); // This is a good idea if you’re transmitting structs.
	if (packet->length != sizeof(Participant))
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
				myHost.participantName[currentClients][i] = s->name[i];
				std::cout << myHost.participantName[currentClients][i];

			}
		}

		for (int j = 0; j < maxCharInIP; j++)
		{
			if (packet->systemAddress.ToString()[j] != NULL)
			{
				myHost.participantIP[currentClients][j] = packet->systemAddress.ToString()[j];
				std::cout << myHost.participantIP[currentClients][j];

			}
			else
			{
				break;
			}
		}
		currentClients++;
	}

	// Perform the functionality for this type of packet, with your struct,  MyStruct *s
}
int main(void)
{
	RakNet::SystemAddress serverAddress;
	bool privateMessage = true;

	//char listOfUsers[maxUsers][maxCharInName];

	char tempName[maxCharInName];
	printf("Please enter you name (must be within 12 Characters):\n");
	fgets(tempName, 12, stdin);

	Host host;
	Participant participant;
	// get key state and then addd the state of teh key board to an array for messaging

	char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;
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
			host.userName[i] = tempName[i];
		}
	}

	if (isServer)
	{
		printf("Starting the chat room for ");
		printf(&host.userName[0]);
		// We need to let the server accept incoming connections from the clients
		serverAddress.FromString("184.171.151.119");
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

		serverAddress.FromString(str);

		std::cout << "\nserver address:" << serverAddress.ToString() << std::endl;

		peer->Connect(str, serverPort, 0, 0);
	}


	// TODO - Add code body here
	while (1)
	{
		Participant* tempPart;
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				printf("Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				printf("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				printf("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:

				printf("Our connection request has been accepted.\n");
				participant.typeId = ID_BROADCAST_USER;
				peer->Send((const char*)&participant, sizeof(participant), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				/*
				participant.nameOfMessageRecipient[0] = 'c';
				participant.nameOfMessageRecipient[1] = 'a';
				participant.nameOfMessageRecipient[2] = 'm';

				//participant.typeId = ID_SEND_PRIVATE_MESSAGE; // for private
				participant.typeId = ID_SEND_PUBLIC_BROADCAST; // for public

				participant.AddToParticipantMsg('p');
				participant.AddToParticipantMsg('l');
				participant.AddToParticipantMsg('s');
				participant.AddToParticipantMsg(' ');
				participant.AddToParticipantMsg('w');
				participant.AddToParticipantMsg('o');
				participant.AddToParticipantMsg('r');
				participant.AddToParticipantMsg('k');

				peer->Send((const char*)& participant, sizeof(participant), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				*/
				// Use a BitStream to write a custom user message
				// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
				//RakNet::BitStream bsOut;

				//WriteStringToBitStream(temp.message, &bsOut);
				//bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				//bsOut.Write(temp.typeId);
				//bsOut.Write(temp.message);
				//peer->Send((const char*)&participant, sizeof(participant), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				//peer->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,packet->systemAddress,true);
				//RakNet::BitStream bsOut;
				//bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				//bsOut.Write("Clients have entered server");

				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				//if (GetPacketIdentifier(packet) == ID_CONNECTION_REQUEST_ACCEPTED/* User assigned packet identifier here */)


				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The chat room is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer) {
					printf("A client has disconnected.\n");
				}
				else {
					printf("We have been disconnected from the host.\n");
				}
				break;
			case ID_CONNECTION_LOST:
				if (isServer) {
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection to the room has been lost.\n");
				}
				break;

			case ID_GAME_MESSAGE_1:
				printf("I am in game message\n");

				if (GetPacketIdentifier(packet) == ID_GAME_MESSAGE_1/* User assigned packet identifier here */)
				{
					//DoMyPacketHandlerClient(packet);
				}

				break;
			case ID_SEND_PRIVATE_MESSAGE:
				// will get the message and send with case ID_RECEIVE_MESSAGE (always)
				// just make sure we send it to the right person whether it is the host or antoher participant
				printf("we are sending a private message");



				break;
			case ID_SEND_PUBLIC_BROADCAST:
				//need all of the usernames
				//need all of the ip address
				//need the message
				printf("we are sending a public broadcast");

				tempPart = (Participant*)packet->data;

				for (int i = 0; i < maxCharInMessage; i++)
				{
					host.messageText[i] = tempPart->message[i];
				}
				host.typeId = ID_RECIEVE_MESSAGE;

				peer->Send((const char*)&host, sizeof(host), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);


				break;
			case ID_RECIEVE_MESSAGE:
				// receives the server's msg and prints to screen
				printf("\n incoming message: \n");
				//DoMyPacketHandlerHost(packet);
				break;
			case ID_BROADCAST_USER:
				if (GetPacketIdentifier(packet) == ID_BROADCAST_USER)
				{
					RecieveClientInfo(packet, host);
				}
				host.typeId = ID_RECIEVE_MESSAGE;

				peer->Send((const char*)&host, sizeof(host), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

				break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}

		}


		if (sendUserMessage)
		{
			sendUserMessage = false;
			// will send the msg
			// will send the name of the person from
			// will need to send who to as well
			// the ID will either be broadcast or private
			// testing:


			if (!isServer)
			{
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
				if (privateMessage)
				{
					participant.typeId = ID_SEND_PRIVATE_MESSAGE;
				}
				else
				{
					participant.typeId = ID_SEND_PUBLIC_BROADCAST;
				}

				peer->Send((const char*)& participant, sizeof(participant), HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
			}
			else
			{
				cout << "\nSending message: \n";
				for (int i = 0; i < maxCharInMessage; i++)
				{
					if (host.messageText[i] == -52)
					{
						break;
					}
					else
					{
						cout << host.messageText[i];
					}
				}
				cout << "\n";

				if (privateMessage)
				{
					//host.typeId = ID_SEND_PRIVATE_MESSAGE;
				}
				else
				{
					host.typeId = ID_SEND_PUBLIC_BROADCAST;
				}
				peer->Send((const char*)& host, sizeof(host), HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
			}

			for (int i = 0; i < maxCharInMessage; i++)
			{
				host.messageText[i] = -52;
				participant.message[i] = -52;
			}
		}


		if (isServer)
		{
			GetInput(host.messageText);
		}
		else
		{
			GetInput(participant.message);
		}

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
		//CheckKeyInput(GetAsyncKeyState(key), key, tempMsg);



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
			//Console.Clear();
			j--;
			tempMsg[j] = -52;

			std::cout << "\b" << tempMsg[j] << "\b";
		}
		pressingKey = true;

	}
	if (GetAsyncKeyState(VK_ESCAPE) != 0)
	{
		if (canTypeMessage)
		{
			//std::cout << "backspace";
			//Console.Clear();
			continueLoop = false;
		}
		pressingKey = true;

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


string GetSystemAddress(Packet* p)
{
	string ip = p->systemAddress.ToString();
	return ip;
}

void CheckKeyInput(bool keyPressed, char charUsed, char msg[])
{
	if (keyPressed)
	{
		for (int i = 0; i < 256; i++)
		{
			if (msg[i] == NULL)
			{
				std::cout << charUsed << "\r";
				msg[i] = charUsed;
				break;
			}
		}
	}
}

unsigned char GetPacketIdentifier(Packet* p)
{
	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	else
		return (unsigned char)p->data[0];
}

// Put this anywhere you want.  Inside the state class that handles the game is a good place
void DoMyPacketHandlerClient(Packet* packet)
{
	// Cast the data to the appropriate type of struct
	Client* s = (Client*)packet->data;
	//	assert(packet->length == sizeof(Client)); // This is a good idea if you’re transmitting structs.
	if (packet->length != sizeof(Client))
	{
		return;
	}
	else
	{
		printf(s->message);
	}

	// Perform the functionality for this type of packet, with your struct,  MyStruct *s
}

void DoMyPacketHandlerParticipant(Packet* packet)
{
	// Cast the data to the appropriate type of struct
	Participant* s = (Participant*)packet->data;
	//	assert(packet->length == sizeof(Client)); // This is a good idea if you’re transmitting structs.
	if (packet->length != sizeof(Participant))
	{
		return;
	}
	else
	{
		printf(s->message);
	}

	// Perform the functionality for this type of packet, with your struct,  MyStruct *s
}

void DoMyPacketHandlerHost(Packet* packet)
{
	// Cast the data to the appropriate type of struct
	Host* s = (Host*)packet->data;
	//	assert(packet->length == sizeof(Client)); // This is a good idea if you’re transmitting structs.
	if (packet->length != sizeof(Host))
	{
		return;
	}
	else
	{
		for (int i = 0; i < maxCharInMessage; i++)
		{
			if (s->messageText[i] == -52)
			{
				break;
			}
			else
			{
				std::cout << s->messageText[i];
			}
		}
	}

	// Perform the functionality for this type of packet, with your struct,  MyStruct *s
}