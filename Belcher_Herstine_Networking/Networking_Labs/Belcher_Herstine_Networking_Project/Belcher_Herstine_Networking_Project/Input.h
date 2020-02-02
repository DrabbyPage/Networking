#ifndef INPUT_H
#define INPUT_H



class Input
{
public:
	Input();
	~Input();

	void GetInput(char tempMsg[]);
	void DisplayConsoleWindow();
	void DisplayHostWindow();

	void setCanTypeMessage(bool mybool);
	bool getCanTypeMessage();
	void setMaxCharInMessage(int mybool);
	int getMaxCharInMessage();
	void setContinueLoop(bool mybool);
	bool getContinueLoop();
	void setIsServer(bool mybool);
	bool getIsServer();
	void setPrintClientsNames(bool mybool);
	bool getPrintClientsNames();
	void setPrivateMessage(bool mybool);
	bool getPrivateMessage();
	void setSendUserMessage(bool mybool);
	bool getSendUserMessage();


	
private:
	bool canTypeMessage;
	int maxCharInMessage;
	bool continueLoop;
	bool isServer;
	bool printClientsNames;
	bool privateMessage;
	bool sendUserMessage;

};







#endif // !Input_H


