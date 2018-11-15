// William Elmes
// Elmes_Randy@yahoo.com
// Bust a Move
// Interactive game based on the arcade game "Bust a Move"

#include "GameBoard.h"

extern GLfloat gColors[7][4];
extern GameBoard gBoard;

Bubble::Bubble(){
	coords.x = -1;
	coords.y = -1;
	color    = EMPTY;
}
Bubble::~Bubble(){
	while( !adjacents.empty() )
		adjacents.pop_front();
}

// Allow Bubble objects to use the = operator
Bubble & Bubble::operator=( const Bubble &rhs ){
	coords.x  = rhs.coords.x;
	coords.y  = rhs.coords.y;
	color     = rhs.color;
	adjacents = rhs.adjacents;

	return *this;
}

void Bubble::Colorize(){
	int randNum = 5;
	while( randNum >= 5 ){
		randNum = ( rand() % 5 ) + 1;
	}
	color = randNum;
}

void Bubble::Draw(){
	// Do not draw Bubbles that have not been initialized or are empty
	if( coords.x == -1 || coords.y == -1 || color == EMPTY )
		return;

	float screenX, screenY, screenRoof = 0.0;
	// coords.x and coords.y are column and row number, respectively
	// Lower rows down towards Launcher as roof lowers, but don't lower Launcher's ammo
	if( coords.y <= gBoard.H + 2 )
		screenRoof = float(gBoard.roofLevel);
	// Indent odd rows 0.5 to the right and center rows on screen
	screenX = float(coords.x) + float(( coords.y + 1 ) % 2 ) * 0.5 - float(gBoard.W) * 0.5 - 0.75;
	// Space columns so that the spheres in the staggered rows only touch, not overlap, and center columsn on screen
	screenY = ( float(coords.y) + screenRoof - 2.5 - float(gBoard.H) * 0.5 ) * rowSpacing * (-1.0);

	// Set color to the Bubble's color, move to the Bubble's screen coordinates, and draw a sphere for the Bubble
	glPushMatrix();
	glColor4f( gColors[color][0], gColors[color][1], gColors[color][2], gColors[color][3] );
	glTranslatef( screenX, screenY, 0.0 );
	glutSolidSphere( 0.5, 32, 32 );
	glPopMatrix();
}

void Bubble::DrawProjectile( float angle, float distance, float x, float y ){
	// Set color to the Bubble's color, move to the Bubble's screen coordinates, and draw a sphere for the Bubble
	glPushMatrix();
	glColor4f( gColors[color][0], gColors[color][1], gColors[color][2], gColors[color][3] );
	glTranslatef( x, y, 0.0 );

	// Turn towards the direction the projectile is traveling (adjust to face upwards) and then move it forward
	glRotatef( angle - 90.0, 0.0, 0.0, 1.0 );
	glTranslatef( 0.0, distance, 0.0 );

	glutSolidSphere( 0.5, 32, 32 );
	glPopMatrix();
}

bool Bubble::TouchingRoof(){
	if( coords.y == 1 )
		return true;
	else
		return false;
}

bool Bubble::TouchingFilled(){
	list<Bubble*>::iterator adj = adjacents.begin();
	Bubble* ptr;
	while( adj != adjacents.end() ){
		ptr = *adj;
		if( !ptr->Empty() )
			return true;
		adj++;
	}
	return false;
}

bool Bubble::DetectCollision( float x, float y, float ang ){
	// Here, we test if a line intersects with a circle
	// x,y are the coords of ammoCurr and represent the starting point of our line
	// trajX,trajY are the coords of a point in the positive direction of ammoCurr's trajectory and represent the ending point of our line
	// ptr->coords.x,y are the coords of the current adjacent's center and represent the center of our circle
	// Note that we are assuming the radius of the circle is 1.0 because the radius of our bubbles is 0.5, so if the centers of two bubbles are within 0.5*2 of each other, they are colliding
	list<Bubble*>::iterator adj = adjacents.begin();
	Bubble* ptr;
	// A point in the positive direction of currAmmo's trajectory that will tell us if there will be a collision in the near future
	float trajX = x + 2 * cos( ang ),
		  trajY = y + 2 * sin( ang ),
		  // a, b, and c from the quadratic equation
		  // a doesn't rely the adjacent's coordinates, so we can calculate ahead of time instead of for every adjacent
		  a = ( trajX - x ) * ( trajX - x ) + ( trajY - y ) * ( trajY - y ),
		  b, c;
	while( adj != adjacents.end() ){
		ptr = *adj;
		if( !ptr->Empty() ){
			b = 2 * ( x * ( trajX - x ) - ptr->coords.x * ( trajX - x ) + y * ( trajY - y ) - ptr->coords.y * ( trajY - y ) );
			c = ( x - ptr->coords.x ) * ( x - ptr->coords.x ) + ( y - ptr->coords.y ) * ( y - ptr->coords.y ) - 1.0;
			// The portion of the quadratic equation under the radical: b^2 - 4ac
			// If this is less than 0, the line between ammoCurr's current position and its future position does not come in range of this adjacent to cause a collision
			// If this = 0, then the line is tangent to the circle and incdicates that ammoCurr grazes this adjacent
			//           ** or one of the endpoints of the line is inside the circle and the other is outside, indicating a collision
			//              either way, we want to indicate a collision here
			// If this is > 0, then the line intersects the circle twice, indicating a collision
			if( b * b - 4 * a * c >= 0 )
				return true;
		}
		adj++;
	}
	return false;
}

bool Bubble::TouchingMatching(){
	list<Bubble*>::iterator adj = adjacents.begin();
	Bubble* ptr;
	while( adj != adjacents.end() ){
		ptr = *adj;
		/*cout << "\tChecking " << ptr->coords.x << "," << ptr->coords.y << ": ";*/
		if( color == ptr->color ){
			/*cout << "Matches!\n";*/
			return true;
		}
		else
			/*cout << "Fail.\n";*/
		adj++;
	}
	return false;
}

bool Bubble::Empty(){
	if( color == 0 )
		return true;
	else
		return false;
}

bool Bubble::IsIn( list<Bubble*> l ){
	list<Bubble*>::iterator ptr = l.begin();
	Bubble* check;
	while( ptr != l.end() ){
		check = *ptr;
		if( ( coords.x == check->coords.x ) && ( coords.y == check->coords.y ) )
			return true;
		ptr++;
	}
	return false;
}

bool Bubble::Matches( Bubble b ){
	if( color == b.color )
		return true;
	else
		return false;
}

void Bubble::Pop(){
	color = EMPTY;
}

void Bubble::Drop(){
	color = EMPTY;
}