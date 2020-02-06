#include "TicTacToe.h"

void TicTacToe::PrintBoard()
{
	int num = 1;
	for (int i = 0; i < 3; i++)
	{
		std::cout << " ";
		for (int j = 0; j < 3; j++)
		{
			std::cout << num<< " ";
			num++;
		}
		std::cout << std::endl;
	}
}


void TicTacToe::PrintTicTacToeGameData(TicTacToeFullGameData tttData)
{
	for (int i = 0; i < 3; i++)
	{
		std::cout << " ";
		for (int j = 0; j < 3; j++)
		{
			std::cout << tttData.gameData[i][j] << " ";
		}
		std::cout << std::endl;
	}
}


// will return X, O or... i think -52 when using this X is for p1 O for p2 -52 for no input
char TicTacToe::CheckTicTacToeWinCondition(TicTacToeFullGameData tttData)
{
	char winner = -52;

	// any of the rows is same
	for (int i = 0; i <= 2; i++)
	{
		if (tttData.gameData[i][0] == tttData.gameData[i][1] && tttData.gameData[i][1] == tttData.gameData[i][2] && tttData.gameData[i][0] != 0)
		{
			winner = tttData.gameData[i][0];
			return winner;
		}
	}

	// any of the columns is same
	for (int i = 0; i <= 2; i++)
	{
		if (tttData.gameData[0][i] == tttData.gameData[1][i] && tttData.gameData[1][i] == tttData.gameData[2][i] && tttData.gameData[0][i] != 0)
		{
			winner = tttData.gameData[0][i];
			return winner;
		}
	}

	// 1st diagonal is same
	if (tttData.gameData[0][0] == tttData.gameData[1][1] && tttData.gameData[1][1] == tttData.gameData[2][2] && tttData.gameData[0][0] != 0)
	{
		winner = tttData.gameData[0][0];
		return winner;
	}

	// 2nd diagonal is same
	if (tttData.gameData[0][2] == tttData.gameData[1][1] && tttData.gameData[1][1] == tttData.gameData[2][0] && tttData.gameData[0][2] != 0)
	{
		winner = tttData.gameData[0][2];
		return winner;
	}

	// if we reached here nobody has won yet

	// if any empty box on tttData.gameData then play on
	for (int i = 0; i <= 2; i++)
	{
		for (int j = 0; j <= 2; j++)
		{
			if (tttData.gameData[i][j] == 0)
			{
				winner = 0;
				return winner;
			}
		}
	}

	return winner;
}