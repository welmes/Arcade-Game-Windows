// William Elmes
// Elmes_Randy@yahoo.com
// Bust a Move
// Interactive game based on the arcade game "Bust a Move"

#include "Bubble.h"

// sqrt(3)/2 -- vertical distance between staggered rows of spheres of radius 0.5 so they just touch
#define rowSpacing 0.8660254038

class GameBoard{
public:
	// W must be greater than 2
	// W should be less than H, and W and H should remain under 24 to keep everything on screen
	static const int W = 10, H = 8;
	static const int GAUGE_MAX = 5;
	int roofLevel, gaugeLevel,
		menu;
	Bubble field[W][H+2];
	bool gameOver;

	GameBoard();
	~GameBoard();
	void Initialize();
	void Draw();
	void RaiseGauge();
	void LowerGauge();
	void AchieveDefeat();
private:
	void SetAdjacents( int c, int r );
	void DrawMenu();
	void DrawControls();
	void DrawInst();
	void DrawBitFont( int x, int y, char *string, void *font );
};