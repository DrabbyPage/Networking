
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
	char name[12];
	char message[120];

	void AddToParticipantMsg(char newChar)
	{
		for (int i = 0; i < 120; i++)
		{
			if (message[i] == NULL)
			{
				message[i] = newChar;
				break;
			}
		}
	}

	void DeleteParticipantLastCharOfMsg()
	{
		for (int i = 0; i < 120; i++)
		{
			if (message[i] == NULL && i != 0)
			{
				message[i - 1] = NULL;
				break;
			}
			else if (i == 119)
			{
				message[119] = NULL;
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
	char userName[12];
	char messageText[120];
	bool privateMsg = false;

	void AddToHostMsg(char newChar)
	{
		for (int i = 0; i < 120; i++)
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
		for (int i = 0; i < 120; i++)
		{
			if (messageText[i] == NULL && i != 0)
			{
				messageText[i - 1] = NULL;
				break;
			}
			else if (i == 119)
			{
				messageText[119] = NULL;
				break;
			}
		}
	}
};
#pragma pack(pop)

//#define MAX_CLIENTS 10
//#define SERVER_PORT 60000

unsigned int maxClients = 1;
unsigned short serverPort = 600;

bool canTypeMessage = true;

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_KEYBOARD_INPUT = ID_USER_PACKET_ENUM + 2,
	ID_BROADCAST_MESSAGE_TO_CLIENTS = ID_USER_PACKET_ENUM + 3
};

unsigned char GetPacketIdentifier(Packet* p);
void DoMyPacketHandlerClient(Packet* packet);
void GetInput(char msg[]);
void CheckKeyInput(bool keyPressed, char charUsed, char msg[]);

int main(void)
{
	//Client temp;
	//temp.typeId = ID_GAME_MESSAGE_1;

	char tempName[12];
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
		for (int i = 0; i < 12; i++)
		{
			participant.name[i] = tempName[i];
		}
	}
	else {
		SocketDescriptor sd(serverPort, 0);
		peer->Startup(maxClients, &sd, 1);
		isServer = true;

		for (int i = 0; i < 12; i++)
		{
			host.userName[i] = tempName[i];
		}
	}



	if (isServer)
	{
		printf("Starting the chat room for ");
		printf(&host.userName[0]);
		printf(".\n");
		// We need to let the server accept incoming connections from the clients
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
		printf(".\n");
		peer->Connect(str, serverPort, 0, 0);
	}


	// TODO - Add code body here
	while (1)
	{
		if (isServer)
		{
			GetInput(host.messageText);
		}
		else
		{
			GetInput(participant.message);
		}

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
				// Use a BitStream to write a custom user message
				// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
				//RakNet::BitStream bsOut;

				//WriteStringToBitStream(temp.message, &bsOut);
				//bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				//bsOut.Write(temp.typeId);
				//bsOut.Write(temp.message);
				participant.typeId = ID_KEYBOARD_INPUT;
				peer->Send((const char*)&participant, sizeof(participant), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				//peer->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,packet->systemAddress,true);
				//RakNet::BitStream bsOut;
				//bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				//bsOut.Write("Clients have entered server");

				break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
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
			case ID_BROADCAST_MESSAGE_TO_CLIENTS:

				break;
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}

			//				if (GetKeyState(VK_RETURN))

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
				for (int i = 0; i < 256; i++)
				{
					if (i == key)
					{
						//std::cout << key;
						canTypeMessage = false;
						tempMsg[j] = key;
						std::cout << tempMsg[j];
						j++;

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
			tempMsg[j] = NULL;

			std::cout << "\b" << tempMsg[j] << "\b";
		}
		pressingKey = true;

	}

	canTypeMessage = !pressingKey;


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