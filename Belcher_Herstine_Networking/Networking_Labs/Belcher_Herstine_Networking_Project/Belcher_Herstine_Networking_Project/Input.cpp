#include "Input.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "RakNet/RakPeerInterface.h"

Input::Input()
{
	canTypeMessage = false;
	maxCharInMessage = false;
	continueLoop = true;
	isServer = false;
	printClientsNames = false;
	privateMessage = false;
	sendUserMessage = false;
}

Input::~Input()
{
}

void Input::DisplayConsoleWindow()
{
	printf("\033[36m");
	printf("-------------------------------------Console Window-------------------------------------\n");
	printf("\033[35m");
	printf("-----ESC: Leave Chat   PAGEUP: Send private message   PAGEDOWN: Send public message-----\n");
	printf("------------------------------TAB: Open Console Window----------------------------------\n");
	printf("\033[0m");
}

void Input::DisplayHostWindow()
{
	printf("\033[31m");
	printf("-------------------------Admin Console Window-------------------------------\n");
	printf("\033[32m");
	printf("----------------ESC: Close Program   TAB: Open Console window---------------\n");
	printf("-------------------CTRL: Print user names and IP addresses------------------\n");
	printf("\033[0m");
}

void Input::setCanTypeMessage(bool mybool)
{
	canTypeMessage = mybool;
}

bool Input::getCanTypeMessage()
{
	return canTypeMessage;
}

void Input::setMaxCharInMessage(int myint)
{
	maxCharInMessage = myint;
}

int Input::getMaxCharInMessage()
{
	return maxCharInMessage;
}

void Input::setContinueLoop(bool mybool)
{
	continueLoop = mybool;
}

bool Input::getContinueLoop()
{
	return continueLoop;
}

void Input::setIsServer(bool mybool)
{
	isServer = mybool;

}

bool Input::getIsServer()
{
	return isServer;
}

void Input::setPrintClientsNames(bool mybool)
{

	printClientsNames = mybool;
}

bool Input::getPrintClientsNames()
{
	return printClientsNames;
}

void Input::setPrivateMessage(bool mybool)
{
	privateMessage = mybool;
}

bool Input::getPrivateMessage()
{
	return privateMessage;
}

void Input::setSendUserMessage(bool mybool)
{
	sendUserMessage = mybool;
}

bool Input::getSendUserMessage()
{
	return sendUserMessage;
}

void Input::GetInput(char tempMsg[])
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
