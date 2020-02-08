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
struct BattleshipShotData
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

	bool GetDoneWithPlacement() { return doneWithPlacement; }

private:
	bool doneWithPlacement = false;
	static const int amountOfShips = 4;
	int currentAmountOfShips = 0;
	ShipData listOfShips[amountOfShips];
	BattleShipFullGameData fullGame;

	// notice this isnt the other persons data its only the data you know
	// AKA when you get hits and misses
	BattleShipFullGameData otherPlayerData;
};


void PrintBattleshipGameData(BattleShipFullGameData bsData);


#endif