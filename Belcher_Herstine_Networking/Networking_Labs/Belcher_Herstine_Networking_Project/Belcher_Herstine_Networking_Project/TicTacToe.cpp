#include "TicTacToe.h"

void PrintTicTacToeGameData(TicTacToeFullGameData tttData)
{
	int num = 1;
	std::cout << std::endl;
	for (int i = 0; i < 3; i++)
	{
		std::cout << " ";
		for (int j = 0; j < 3; j++)
		{
			if (tttData.gameData[i][j] != -52)
			{
				std::cout << tttData.gameData[i][j] << " ";
			}
			else
			{
				std::cout << num << " ";
			}
			num++;
		}
		std::cout << std::endl;
	}
}


// will return X, O or... i think -52 when using this X is for p1 O for p2 -52 for no input
char CheckTicTacToeWinCondition(TicTacToeFullGameData tttData)
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


// takes in the pos and checks to see if the pos is open for input
// will return false if the move is not able to be made or if the input was not 1-9
bool AddInputToTicTacToeGame(char newPos, char player,TicTacToeFullGameData tttData)
{
	switch (newPos)
	{
	case '1':
		if (tttData.gameData[0][0] == -52)
		{
			tttData.gameData[0][0] = player;
		}
		else
		{
			// they cant make that move
			return false;
		}
		break;
	case '2':
		if (tttData.gameData[0][1] == -52)
		{
			tttData.gameData[0][1] = player;
		}
		else
		{
			// they cant make that move
			return false;
		}
		break;
	case '3':
		if (tttData.gameData[0][2] == -52)
		{
			tttData.gameData[0][2] = player;
		}
		else
		{
			// they cant make that move
			return false;
		}
		break;
	case '4':
		if (tttData.gameData[1][0] == -52)
		{
			tttData.gameData[1][0] = player;
		}
		else
		{
			// they cant make that move
			return false;
		}
		break;
	case '5':
		if (tttData.gameData[1][1] == -52)
		{
			tttData.gameData[1][1] = player;
		}
		else
		{
			// they cant make that move
			return false;
		}
		break;
	case '6':
		if (tttData.gameData[1][2] == -52)
		{
			tttData.gameData[1][2] = player;
		}
		else
		{
			// they cant make that move
			return false;
		}
		break;
	case '7':
		if (tttData.gameData[2][0] == -52)
		{
			tttData.gameData[2][0] = player;
		}
		else
		{
			// they cant make that move
			return false;
		}
		break;
	case '8':
		if (tttData.gameData[2][1] == -52)
		{
			tttData.gameData[2][1] = player;
		}
		else
		{
			// they cant make that move
			return false;
		}
		break;
	case '9':
		if (tttData.gameData[2][2] == -52)
		{
			tttData.gameData[2][2] = player;
		}
		else
		{
			// they cant make that move
			return false;
		}
		break;
	default:
		//they entered something wrong
		return false;
		break;
	}
	return true;
}
