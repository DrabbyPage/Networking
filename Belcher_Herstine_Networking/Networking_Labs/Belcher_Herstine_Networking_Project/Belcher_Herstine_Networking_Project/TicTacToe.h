#include <iostream>
#include "GameMessageEnums.h"
#ifndef TIC_TAC_TOE
#define TIC_TAC_TOE

class TicTacToe
{
	// type id should ALWAYS BE RECEIVE TIC_TAC_TOE_FULL
	struct TicTacToeFullGameData
	{
		unsigned char typeId;
		char gameData[3][3];
	};
	// type id should always be ID_RECEIVE_TIC_TAC_TOE
	struct TicTacToeData
	{
		unsigned char typeId;
		char ticTacToePos; // would be a... number?
	};


public:
	void PrintBoard();
	void PrintTicTacToeGameData(TicTacToeFullGameData tttData);
	// will return X, O or... i think -52 when using this X is for p1 O for p2 -52 for no input
	char CheckTicTacToeWinCondition(TicTacToeFullGameData tttData);


};
#endif
