// William Elmes
// Elmes_Randy@yahoo.com
// Bust a Move
// Interactive game based on the arcade game "Bust a Move"

#include "Launcher.h"

extern Launcher gLauncher;

GameBoard::GameBoard(){
	menu = 0;
	gameOver = true;

	for( int c = 1; c <= W; c++ ){
		for( int r = 1; r <= H+2; r++ ){
			// Set coordinates for each Bubble in the field grid
			field[c-1][r-1].coords.x = c;
			field[c-1][r-1].coords.y = r;
			// Set up the adjacents list for each Bubble in the field grid
			SetAdjacents( c, r );
		}
	}
}

GameBoard::~GameBoard(){

}

void GameBoard::Initialize(){
	gameOver = false;
	roofLevel  = 0;
	gaugeLevel = 0;

	srand( time(NULL) );
	for( int c = 1; c <= W; c++ ){
		for( int r = 1; r <= H+2; r++ ){
			// Only color first 3/4 rows, leave space for launched Bubbles
			if( r < int( ceil( float(H) * 0.75 )) )
				field[c-1][r-1].Colorize();
			else
				field[c-1][r-1].color = 0;
		}
	}
	gLauncher.Initialize();
}

void GameBoard::SetAdjacents( int c, int r ){
	Bubble* ptr;

	// Add all nodes adjacent to the node at column c, row r to its adjacents list
	// Note: c and r refer to column and row numbers and start at 1
	// field matrix indices start at 0, so we have to subtract 1 when choosing the right element of field
	if( c > 1 ){
		// Mid left - =
		ptr = &field[c-2][r-1];
		field[c-1][r-1].adjacents.push_back( ptr );
	}
	if( c < W ){
		// Mid right + =
		ptr = &field[c][r-1];
		field[c-1][r-1].adjacents.push_back( ptr );
	}
	// Odd rows
	if( r % 2 == 1 ){
		if( r > 1 ){
			if( c > 1 ){
				// Top left - -
				ptr = &field[c-2][r-2];
				field[c-1][r-1].adjacents.push_back( ptr );
			}
			// Top right = -
			ptr = &field[c-1][r-2];
			field[c-1][r-1].adjacents.push_back( ptr );
		}
		if( r < H + 2 ){
			if( c > 1 ){
				// Bottom left - +
				ptr = &field[c-2][r];
				field[c-1][r-1].adjacents.push_back( ptr );
			}
			// Bottom right = +
			ptr = &field[c-1][r];
			field[c-1][r-1].adjacents.push_back( ptr );
		}
	}
	// Even rows
	else{
		// Top left = -
		ptr = &field[c-1][r-2];
		field[c-1][r-1].adjacents.push_back( ptr );
		if( c < W ){
			// Top right + -
			ptr = &field[c][r-2];
			field[c-1][r-1].adjacents.push_back( ptr );
			if( r < H + 2 ){
				// Bottom right + +
				ptr = &field[c][r];
				field[c-1][r-1].adjacents.push_back( ptr );
			}
		}
		if( r < H + 2 ){
			// Bottom left = +
			ptr = &field[c-1][r];
			field[c-1][r-1].adjacents.push_back( ptr );
		}
	}
}

void GameBoard::Draw(){
	// Print lose message
	if( menu == 3 ){
		glPushMatrix();
		glTranslatef( 0.0, 0.0, 1.0 );
		glColor4f( 0.1, 0.1, 0.1, 1.0 );
		glBegin( GL_QUADS );
		glVertex3f( -1.5, 2.5, 0.0 );
		glVertex3f( -1.5, 0.5, 0.0 );
		glVertex3f(  1.5, 0.5, 0.0 );
		glVertex3f(  1.5, 2.5, 0.0 );
		glEnd();
		glColor4f( 1.0, 1.0, 1.0, 1.0 );
		DrawBitFont( -1, 2, "You lose!",	 GLUT_BITMAP_TIMES_ROMAN_24 );
		DrawBitFont( -1, 1, "Press any key", GLUT_BITMAP_TIMES_ROMAN_24 );
		glPopMatrix();
	}
	// Print win message
	else if( menu == 4 ){
		glPushMatrix();
		glTranslatef( 0.0, 0.0, 1.0 );
		glColor4f( 0.1, 0.1, 0.1, 1.0 );
		glBegin( GL_QUADS );
		glVertex3f( -1.5, 2.5, 0.0 );
		glVertex3f( -1.5, 0.5, 0.0 );
		glVertex3f(  1.5, 0.5, 0.0 );
		glVertex3f(  1.5, 2.5, 0.0 );
		glEnd();
		glColor4f( 1.0, 1.0, 1.0, 1.0 );
		DrawBitFont( -1, 2, "You win!",		 GLUT_BITMAP_TIMES_ROMAN_24 );
		DrawBitFont( -1, 1, "Press any key", GLUT_BITMAP_TIMES_ROMAN_24 );
		glPopMatrix();
	}

	// Draw game menu instead if the game is over or has not yet started
	if( !gameOver ){
		for( int c = 0; c < W; c++ ){
			for( int r = 0; r < H + 2; r++ ){
				field[c][r].Draw();
			}
		}
	}
	else{
		switch( menu ){
			case 0:
				DrawMenu();
				break;
			case 1:
				DrawControls();
				break;
			case 2:
				DrawInst();
				break;
		}
	}
}

void GameBoard::DrawMenu(){
	glColor4f( 1.0, 1.0, 1.0, 1.0 );
	DrawBitFont( -1,  5, "Bust a Move",				   GLUT_BITMAP_TIMES_ROMAN_24 );
	DrawBitFont( -2,  2, "Select a numerical option:", GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -2,  1, "1. Start new game",		   GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -2,  0, "2. View instructions",	   GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -2, -1, "3. Exit game",	           GLUT_BITMAP_HELVETICA_18   );
}

void GameBoard::DrawControls(){
	DrawBitFont( -1,  5, "Bust a Move",				   GLUT_BITMAP_TIMES_ROMAN_24 );
	DrawBitFont( -1,  2, "Controls",				   GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -3,  1, "Left Arrow Key:",			   GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont(  0,  1, "Rotate aimer left",		   GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -3,  0, "Right Arrow Key:",		   GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont(  0,  0, "Rotate aimer right",		   GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -3, -1, "Spacebar:",				   GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont(  0, -1, "Launch bubble",			   GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -2, -6, "Press any key to continue",  GLUT_BITMAP_HELVETICA_18   );
}

void GameBoard::DrawInst(){
	DrawBitFont( -1,  5, "Bust a Move",				   GLUT_BITMAP_TIMES_ROMAN_24 );
	DrawBitFont( -6,  4, "The goal of the game is to pop all of the bubbles.",											 GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -6,  3, "Launch bubbles at the roof from your aimer at the bottom center of the screen.",				 GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -6,  2, "Your aimer shows the color of your current bubble; the next bubble is in the right corner.",	 GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -6,  1, "Launched bubbles will land and stop moving when they either hit another bubble or the roof.",  GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -6,  0, "If a bubble lands in a group of at least 3 matching-color bubbles, they will all be popped.",	 GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -6, -1, "If any bubbles are left floating after its neighbors are popped, it will be popped as well.",	 GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -6, -2, "The gauge in the bottom left raises on launches, but drops when a floating bubble is popped.", GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -6, -3, "If the gauge overflows, it will push the roof and all bubbles down and reset the gauge.",		 GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -6, -4, "If a bubble lands or is pushed belows the dotted threshhold, you lose the game.",				 GLUT_BITMAP_HELVETICA_18   );
	DrawBitFont( -2, -6, "Press any key to continue",  GLUT_BITMAP_HELVETICA_18   );
}

void GameBoard::DrawBitFont( int x, int y, char *string, void *font ){
	int len;
	glRasterPos2f( x, y );
	len = int(strlen( string ));
	for( int i = 0; i < len; i++ ){
		glutBitmapCharacter( font, string[i] );
	}
}

void GameBoard::RaiseGauge(){
	gaugeLevel++;
	/*cout << "Gauge raised:\t" << gaugeLevel << endl;*/
	if( gaugeLevel >= GAUGE_MAX ){
		gaugeLevel -= GAUGE_MAX;
		roofLevel++;
		/*cout << "\tRoof lowered: " << roofLevel << endl;*/
		for( int i = 0; i < W; i++ ){
			if( !field[i][H - roofLevel + 1].Empty() ){
				cout << "Loser!\n";
				AchieveDefeat();
				return;
			}
		}
	}
}

void GameBoard::LowerGauge(){
	if( gaugeLevel > 0 )
		gaugeLevel--;
	/*cout << "Gauge lowered:\t" << gaugeLevel << endl;*/
}

void GameBoard::AchieveDefeat(){
	menu = 3;
}