#include <iostream>
#include "GameMessageEnums.h"
#ifndef TIC_TAC_TOE
#define TIC_TAC_TOE

// type id should ALWAYS BE RECEIVE TIC_TAC_TOE_FULL
struct TicTacToeFullGameData
{
	unsigned char typeId;
	char gameData[3][3];
	void FillTicTacToe()
	{
		typeId = ID_RECEIVE_TICTACTOE_FULL;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				gameData[i][j] = -52;
			}
		}
	}
};
// type id should always be ID_RECEIVE_TIC_TAC_TOE
struct TicTacToeData
{
	unsigned char typeId;
	char ticTacToePos; // would be a... number?
};

void PrintTicTacToeGameData(TicTacToeFullGameData tttData);
// will return X, O or... i think -52 when using this X is for p1 O for p2 -52 for no input
char CheckTicTacToeWinCondition(TicTacToeFullGameData tttData);

//void FillTicTacToe(TicTacToeFullGameData tttData);

#endif
