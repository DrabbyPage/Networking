
#include "Battleship.h"

int GivePositionXFromChar(char xChar);
int GivePositionYFromChar(char yChar);

BattleshipManager::BattleshipManager()
{

}

BattleshipManager::~BattleshipManager()
{


}

// adds a new ship to the list given the xpos ypos and whether it is horizontal or vertical
// it adds based on the size and if the user tries to place the ship out of bounds then the 
//chips tiles will be placed ahead of the starting tile pos
void BattleshipManager::AddShip(char xPos, char yPos, bool isHorizontal, int newSizeOfShip)
{
	for (int i = 0; i < currentAmountOfShips; i++)
	{
		// make sure the ship size is valid and not a duplicate
		if (listOfShips[i].sizeOfShip == newSizeOfShip &&
			newSizeOfShip < 6 && newSizeOfShip > 1)
		{
			// the is an incorrect placement
			std::cout << "\nThat is an incorrect size\n";
			return;
		}
	}

	BattleshipPos newPos;
	newPos.isHit = false;
	newPos.posX = xPos;
	newPos.posY = yPos;

	// get the x and y int values for easy checking
	int xPosInt = GivePositionXFromChar(xPos);
	int yPosInt = GivePositionYFromChar(yPos);

	ShipData newShip;
	newShip.sizeOfShip = newSizeOfShip;
	newShip.isSunk = false;

	int inFrontOfShipVal = 1;

	for (int i = 0; i < newSizeOfShip; i++)
	{
		if (isHorizontal)
		{
			// if the next pos is out of bounds
			if (xPosInt + i >= 10)
			{
				//put the position in front of the place location
				xPosInt = xPosInt - inFrontOfShipVal;
				newPos.posX = newPos.posX - inFrontOfShipVal;
				inFrontOfShipVal--;
				newPos.posY = yPos;

				// check to see that placement is not on another ship
				for (int k = 0; k < currentAmountOfShips; k++)
				{
					for (int j = 0; j < listOfShips[k].tilePos.size(); j++)
					{
						if (newPos.posX == listOfShips[k].tilePos.at(j).posX && yPos == listOfShips[k].tilePos.at(j).posY)
						{
							std::cout << "\nThat placement is already taken by another ship. Try Again.\n";
							return;
						}
					}
				}

				// add the ship to the game board
				fullGame.gameData[yPosInt][xPosInt] = '#';
				newShip.tilePos.push_back(newPos);
			}
			else
			{
				xPosInt = xPosInt + i;
				newPos.posX += i;
				newPos.posY = yPos;

				// check to see that placement is not on another ship
				for (int k = 0; k < currentAmountOfShips; k++)
				{
					for (int j = 0; j < listOfShips[k].tilePos.size(); j++)
					{
						if (newPos.posX == listOfShips[k].tilePos.at(j).posX && yPos == listOfShips[k].tilePos.at(j).posY)
						{
							std::cout << "\nThat placement is already taken by another ship. Try Again.\n";
							return;
						}
					}
				}

				// add the ship to the game board
				fullGame.gameData[yPosInt][xPosInt] = '#';
				newShip.tilePos.push_back(newPos);
			}
		}
		else
		{
			if (yPosInt + i >= 10)
			{
				newPos.posY = newPos.posY - inFrontOfShipVal;
				yPosInt = yPosInt - inFrontOfShipVal;
				inFrontOfShipVal--;
				newPos.posX = xPos;

				// check to see that placement is not on another ship
				for (int k = 0; k < currentAmountOfShips; k++)
				{
					for (int j = 0; j < listOfShips[k].tilePos.size(); j++)
					{
						if (xPos == listOfShips[k].tilePos.at(j).posX && newPos.posY == listOfShips[k].tilePos.at(j).posY)
						{
							std::cout << "\nThat placement is already taken by another ship. Try Again.\n";
							return;
						}
					}
				}

				// add the ship to the game board
				fullGame.gameData[yPosInt][xPosInt] = '#';
				newShip.tilePos.push_back(newPos);
			}
			else
			{
				newPos.posY += i;
				yPosInt = yPosInt + i;
				newPos.posX = xPos;

				// check to see that placement is not on another ship
				for (int k = 0; k < currentAmountOfShips; k++)
				{
					for (int j = 0; j < listOfShips[k].tilePos.size(); j++)
					{
						if (xPos == listOfShips[k].tilePos.at(j).posX && newPos.posY == listOfShips[k].tilePos.at(j).posY)
						{
							std::cout << "\nThat placement is already taken by another ship. Try Again.\n";
							return;
						}
					}
				}

				// add the ship to the game board
				fullGame.gameData[yPosInt][xPosInt] = '#';
				newShip.tilePos.push_back(newPos);
			}
		}
	}

	listOfShips[currentAmountOfShips] = newShip;

	currentAmountOfShips++;

	if (currentAmountOfShips == 4)
	{
		doneWithPlacement = true;
	}
}

// checks to see if all ships have sunken
// (for win/loss state)
bool BattleshipManager::CheckLossOfPlayer()
{
	bool allSunk = true;
	for (int i = 0; i < 4; i++)
	{
		if (!listOfShips[i].isSunk)
		{
			allSunk = false;
			break;
		}
	}

	return allSunk;
}

// this function takes in two char for position then checks for a hit at that position
// if there is a hit it returns true... so we can use that info later on
// (sending feedback on a hit)
bool BattleshipManager::CheckHitOfShip(char shotPosX, char shotPosY)
{
	// check the list of the ships
	for (int i = 0; i < amountOfShips; i++)
	{
		// make sure that the ship is not already sunk
		if (!listOfShips[i].isSunk)
		{
			// loop through tiles to see if the pos is a hit
			for (int j = 0; j < listOfShips[i].tilePos.size(); j++)
			{
				// check if the pos is the same as the tile's x val
				if (listOfShips[i].tilePos[j].posX == shotPosX)
				{
					// check if the pos is the same as the tiles y val
					if (listOfShips[i].tilePos[j].posY == shotPosY)
					{
						// make the tile officially hit
						listOfShips[i].tilePos[j].isHit = true;

						// get the 2d array coordinates for feedback
						int hitPosX = GivePositionXFromChar(shotPosX);
						int hitPosY = GivePositionYFromChar(shotPosY);

						// show the player that was hit that they were hit (NOT THE SHOOTING PLAYER YET)
						fullGame.gameData[hitPosY][hitPosX] = 'X';

						// check to see if the ship was sunk
						for (int k = 0; k < listOfShips[i].tilePos.size(); k++)
						{
							if (listOfShips[i].tilePos.at(k).isHit)
							{
								listOfShips[i].isSunk = true;
							}
							else
							{
								listOfShips[i].isSunk = false;
								break;
							}
						}

						return true;
					}
				}
			}
		}

	}

	// show the missed shot to the receiving player (not the shooting player)
	int missPosX = GivePositionXFromChar(shotPosX);
	int missPosY = GivePositionYFromChar(shotPosY);

	fullGame.gameData[missPosY][missPosX] = 'O';

	return false;
}

// this function will be used at the beginning after the player has shown which tiles 
// are the ships positions
void BattleshipManager::FillBSFullGameData()
{
	// fill the rest of the list with water!!!
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			fullGame.gameData[i][j] = '~';
		}
	}
}

// letters 1,2,3,4,etc are linked to their integer variables
int GivePositionXFromChar(char xChar)
{
	int posXOfShip = -1;
	switch (xChar)
	{
	case '0':
		posXOfShip = 0;
		break;
	case '1':
		posXOfShip = 1;
		break;
	case '2':
		posXOfShip = 2;
		break;
	case '3':
		posXOfShip = 3;
		break;
	case '4':
		posXOfShip = 4;
		break;
	case '5':
		posXOfShip = 5;
		break;
	case '6':
		posXOfShip = 6;
		break;
	case '7':
		posXOfShip = 7;
		break;
	case '8':
		posXOfShip = 8;
		break;
	case '9':
		posXOfShip = 9;
		break;
	}
	return posXOfShip;
}

// letters a,b,c,etc are linked to 0,1,2,3,etc corrdinates
int GivePositionYFromChar(char yChar)
{
	int posYOfShip = -1;
	switch (yChar)
	{
	case 'A':
	case'a':
		posYOfShip = 0;
		break;
	case 'B':
	case 'b':
		posYOfShip = 1;
		break;
	case 'C':
	case 'c':
		posYOfShip = 2;
		break;
	case 'D':
	case 'd':
		posYOfShip = 3;
		break;
	case 'E':
	case 'e':
		posYOfShip = 4;
		break;
	case 'F':
	case 'f':
		posYOfShip = 5;
		break;
	case 'G':
	case 'g':
		posYOfShip = 6;
		break;
	case 'H':
	case 'h':
		posYOfShip = 7;
		break;
	case 'I':
	case 'i':
		posYOfShip = 8;
		break;
	case 'J':
	case 'j':
		posYOfShip = 9;
		break;
	}
	return posYOfShip;
}

// prints data of the game like:
//  1 2 3 4
// a 
// b
// c
void PrintBattleshipGameData(BattleShipFullGameData bsData)
{
	// 1 2 3 4 5 6 7 8 (the top of the battle ship game
	std::cout << "  ";
	for (unsigned int i = 0; i < 9; i++)
	{
		std::cout << i << " ";
	}
	std::cout << std::endl;
	//
	// a b c d e f g h
	char side = 'a';
	for (unsigned int i = 0; i < 9; i++)
	{
		// a b c d e f g h (side) (the start of teh line that is)
		std::cout << side << " ";

		// orint the rest of the line data
		for (int j = 0; j < 9; j++)
		{
			std::cout << bsData.gameData[i][j] << " ";
		}
		std::cout << std::endl;
		side++;
	}
}