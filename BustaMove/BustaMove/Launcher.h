// William Elmes
// Elmes_Randy@yahoo.com
// Bust a Move
// Interactive game based on the arcade game "Bust a Move"

#include "GameBoard.h"

class Launcher{
public:
	float angleAim;

	Launcher();
	~Launcher();
	void Initialize();
	void Draw();
	void Launch();
	void TimerCount();
	void PrintLists();
private:
	static const int POP_SIZE_REQUIREMENT = 3;
	float angleLaunch,
		  timerLaunch,
		  baseX, baseY,
		  currX, currY,
		  nextX, nextY;
	bool hasBounced, loaded;
	int launchPopped;
	Bubble ammoCurr, ammoNext;
	list<Bubble*> popCheck;
	list<Bubble*> pop;
	list<Bubble*> fallCheck;
	list<Bubble*> fall;
	list<Bubble*> safe;
	list<Bubble*> group;
	list<Bubble*> groupCheck;

	void LoadNext();
	void Ricochet();
	bool AtLanding();
	void ChainPop();
	void PopMatching();
	void ChainFall();
	void AchieveVictory();
};