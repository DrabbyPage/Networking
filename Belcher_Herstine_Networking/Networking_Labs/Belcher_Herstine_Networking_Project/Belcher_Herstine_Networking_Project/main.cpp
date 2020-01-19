
#include <iostream>
#include <stdio.h>
#include <string.h>
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
	const char* message;
};
#pragma pack(pop)

//#define MAX_CLIENTS 10
//#define SERVER_PORT 60000

unsigned int maxClients = 1;
unsigned short serverPort = 600;


enum GameMessages
{
	ID_GAME_MESSAGE_1=ID_USER_PACKET_ENUM+1
};



unsigned char GetPacketIdentifier(Packet *p)
{
	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	else
		return (unsigned char) p->data[0];
}

// Put this anywhere you want.  Inside the state class that handles the game is a good place
void DoMyPacketHandler(Packet *packet)
{
	// Cast the data to the appropriate type of struct
	Client *s = (Client *) packet->data;
	assert(packet->length == sizeof(Client)); // This is a good idea if you’re transmitting structs.
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

int main(void)
{
	Client temp;
	temp.message = "hewwo";
	temp.typeId = ID_GAME_MESSAGE_1;

	char str[512];
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;
	RakNet::Packet* packet;

	printf("(C) or (S)erver?\n");
	fgets(str, 512, stdin);
	if ((str[0] == 'c') || (str[0] == 'C'))
	{
		SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;
	}
	else {
		SocketDescriptor sd(serverPort, 0);
		peer->Startup(maxClients, &sd, 1);
		isServer = true;
	}



	if (isServer)
	{
		printf("Starting the server.\n");
		// We need to let the server accept incoming connections from the clients
		peer->SetMaximumIncomingConnections(maxClients);
	}
	else {
		printf("Enter server IP or hit enter for 127.0.0.1\n");
		fgets(str, 512, stdin);
		if (str[0] == 0) {
			strcpy(str, "127.0.0.1");
		}
		printf("Starting the client.\n");
		peer->Connect(str, serverPort, 0, 0);

	}

	// TODO - Add code body here
	while (1)
	{
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
			{
				printf("Our connection request has been accepted.\n");

				// Use a BitStream to write a custom user message
				// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
				RakNet::BitStream bsOut;

				//WriteStringToBitStream(temp.message, &bsOut);
				//bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				//bsOut.Write(temp.typeId);
				//bsOut.Write(temp.message);
				peer->Send((const char*)& temp, sizeof(temp), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				//peer->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,packet->systemAddress,true);
				//RakNet::BitStream bsOut;
				//bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				//bsOut.Write("Clients have entered server");
			}
			break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer) {
					printf("A client has disconnected.\n");
				}
				else {
					printf("We have been disconnected.\n");
				}
				break;
			case ID_CONNECTION_LOST:
				if (isServer) {
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection lost.\n");
				}
				break;

			case ID_GAME_MESSAGE_1:
			{
				
				printf("I am in game message\n");

				if (GetPacketIdentifier(packet) == ID_GAME_MESSAGE_1/* User assigned packet identifier here */)
				{
					DoMyPacketHandler(packet);
				}
//				RakNet::RakString rs;
//				RakNet::BitStream bsIn(packet->data, packet->length, true);
//				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
//				bsIn.Read(rs);
//				printf("%s\n", rs.C_String());
//
//				RakNet::BitStream bsOut;
//				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);

			}
			break;

			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}

			//			if (isServer)
			//			{
			//				RakNet::BitStream bsOut;
			//				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
			//
			//				printf("I am the server");
			//
			//				if (GetPacketIdentifier(packet) == ID_GAME_MESSAGE_1/* User assigned packet identifier here */)
			//				{
			//					DoMyPacketHandler(packet);
			//				}
			//				//	fgets(str, 512, stdin);
			//				if (str[0] == 'A' || str[0] == 'a')
			//				{
			//					printf("we in \n");
			//					bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
			//					bsOut.Write("Youre a bitch");
			//					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			//				}
			//			}
			//			else
			//			{
			//
			//				printf("Please press enter:\n");
			//
			//				fgets(str, 512, stdin);
			//
			//				if (GetKeyState(VK_RETURN))
			//				{
			//					RakNet::BitStream bsOut;
			//
			//					//WriteStringToBitStream(temp.message, &bsOut);
			//					//bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
			//					//bsOut.Write(temp.typeId);
			//					//bsOut.Write(temp.message);
			//					peer->Send((const char*)& temp, sizeof(temp), HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			//					//peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			//				}
			//			}
		}


	}

	printf("im done lol");

	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}