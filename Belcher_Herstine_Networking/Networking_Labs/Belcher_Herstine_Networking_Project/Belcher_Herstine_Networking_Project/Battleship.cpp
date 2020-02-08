
#include "Battleship.h"

int GivePositionXFromChar(char xChar);
int GivePositionYFromChar(char yChar);

BattleshipManager::BattleshipManager()
{

}

BattleshipManager::~BattleshipManager()
{


}

// adds a new ship to the list
void BattleshipManager::AddShip(char xPos, char yPos, bool isHorizontal, int newSizeOfShip)
{
	BattleshipPos newPos;
	newPos.isHit = false;
	newPos.posX = xPos;
	newPos.posY = yPos;

	int xPosInt = GivePositionXFromChar(xPos);
	int yPosInt = GivePositionXFromChar(yPos);

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
				newPos.posX -= inFrontOfShipVal;
				inFrontOfShipVal--;
			}
			else
			{
				newPos.posX += i;
				newShip.tilePos.push_back(newPos);
			}
			newPos.posY = yPos;
		}
		else
		{
			if (yPosInt + i >= 10)
			{
				newPos.posY -= inFrontOfShipVal;
				inFrontOfShipVal--;
			}
			else
			{
				newPos.posY += i;
				newShip.tilePos.push_back(newPos);
			}
			newPos.posX = xPos;
		}
	}

	listOfShips[currentAmountOfShips] = newShip;
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
		if (!listOfShips->isSunk)
		{
			// loop through tiles to see if the pos is a hit
			for (int j = 0; j < listOfShips->tilePos.size(); j++)
			{
				// check if the pos is the same as the tile's x val
				if (listOfShips->tilePos[j].posX == shotPosX)
				{
					// check if the pos is the same as the tiles y val
					if (listOfShips->tilePos[j].posY == shotPosY)
					{
						// make the tile officially hit
						listOfShips->tilePos[j].isHit = true;

						// get the 2d array coordinates for feedback
						int hitPosX = GivePositionXFromChar(shotPosX);
						int hitPosY = GivePositionYFromChar(shotPosY);

						// show the player that was hit that they were hit (NOT THE SHOOTING PLAYER YET)
						fullGame.gameData[hitPosY][hitPosX] = 'X';

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
	int posXOfShip;
	int posYOfShip;
	// go through each list of ship point... fill the pos of ships
	for (int i = 0; i < amountOfShips; i++)
	{
		for (int j = 0; j < listOfShips[i].tilePos.size(); j++)
		{
			posXOfShip = GivePositionXFromChar(listOfShips[i].tilePos.at(j).posX);

			posYOfShip = GivePositionYFromChar(listOfShips[i].tilePos.at(j).posY);

			fullGame.gameData[posYOfShip][posXOfShip] = '#';
		}
	}

	// fill the rest of the list with water!!!
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (fullGame.gameData[i][j] != '#' )
			{
				fullGame.gameData[i][j] = '~';
			}
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


void PrintBattleshipGameData(BattleShipFullGameData bsData)
{
	// 1 2 3 4 5 6 7 8 (the top of the battle ship game
	std::cout << " ";
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