#include <vector>
#include <iostream>

#ifndef BATTLESHIP
#define BATTLESHIP

// should always be type id ID_RECEIVE_BATTLESHIP_FULL !!!
#pragma pack(push, 1)
struct BattleShipFullGameData
{
	unsigned char typeId;
	char gameData[10][10]; // [letter] [number]
};
#pragma pack(pop)


//should always be type id ID_RECEIVE_BATTLESHIP !!!
#pragma pack(push, 1)
struct BattleshipData
{
	unsigned char typeId;
	char yPos; // has to be a letter
	char xPos; // has to be a Number
};
#pragma pack(pop)


struct BattleshipPos
{
	char posX;
	char posY;
	bool isHit;
};


struct ShipData
{
	bool isSunk;
	std::vector<BattleshipPos>tilePos;
	int sizeOfShip; // value between 3-5
};


class BattleshipManager
{
public:
	BattleshipManager();
	~BattleshipManager();

	void AddShip(char xPos, char yPos, bool isHorizontal, int newSizeOfShip);
	bool CheckLossOfPlayer();
	bool CheckHitOfShip(char shotPosX, char shotPosY);
	void FillBSFullGameData();

private:
	static const int amountOfShips = 4;
	int currentAmountOfShips = 0;
	ShipData listOfShips[amountOfShips];
	BattleShipFullGameData fullGame;
};


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


#endif