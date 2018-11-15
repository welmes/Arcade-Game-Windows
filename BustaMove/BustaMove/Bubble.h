// William Elmes
// Elmes_Randy@yahoo.com
// Bust a Move
// Interactive game based on the arcade game "Bust a Move"

#include <list>
#include <math.h>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include "glut.h"
#include <string>

using namespace std;

class Bubble{
public:
	enum { EMPTY, RED, GREEN, BLUE, YELLOW, TURQ, VIOLET };
	struct Coords{ int x, y; };

	int color;
	Coords coords;
	list<Bubble*> adjacents;

	Bubble();
	~Bubble();
	Bubble & operator=( const Bubble &rhs );
	void Colorize();
	void Draw();
	void DrawProjectile( float angle, float distance, float x, float y );
	bool TouchingRoof();
	bool TouchingFilled();
	bool DetectCollision( float x, float y, float a );
	bool TouchingMatching();
	bool Empty();
	bool IsIn( list<Bubble*> l );
	bool Matches( Bubble b );
	void Pop();
	void Drop();
};