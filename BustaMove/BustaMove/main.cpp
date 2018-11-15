// William Elmes
// Elmes_Randy@yahoo.com
// Bust a Move
// Interactive game based on the arcade game "Bust a Move"

#include "Launcher.h"

void Init();
void Display();
void Reshape( int w, int h );
void Mouse( int button, int state, int x, int y );
void Keyboard( unsigned char key, int x, int y );
void Arrowkeys( int key, int x, int y );


GLfloat gColors[7][4] = { { 0.1, 0.1, 0.1, 0.1 },   // EMPTY
						  { 1.0, 0.0, 0.0, 1.0 },   // RED
						  { 0.0, 1.0, 0.0, 1.0 },   // GREEN
						  { 0.1, 0.1, 1.0, 1.0 },   // BLUE
						  { 1.0, 1.0, 0.0, 1.0 },   // YELLOW
						  { 1.0, 0.0, 1.0, 1.0 },   // TURQ
						  { 0.0, 1.0, 1.0, 1.0 } }; // VIOLET
Launcher gLauncher;
GameBoard gBoard;


int main(){
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( 800, 800 );
	glutInitWindowPosition( 100, 100 );
	int mainWin = glutCreateWindow( "Bust a Move" );

	Init();

	glutDisplayFunc( Display );
	glutReshapeFunc( Reshape );
	glutMouseFunc( Mouse );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( Arrowkeys );

	glutMainLoop();

	return 0;
}


void Init(){
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glShadeModel( GL_SMOOTH );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_COLOR_MATERIAL );
}

void Display(){
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glPushMatrix();
	glLoadIdentity();
	// Adjust the distance the camera is from the origin to keep all objects on screen
	// Use a default zoom distance for game menus when a game is not in session
	float h = 11.0;
	if( !gBoard.gameOver )
		h = float(gBoard.H);
	gluLookAt( 0.0, 0.0, 4.5 + h*1.05,
			   0.0, 0.0, 0.0,
			   0.0, 1.0, 0.0 );

	glPushMatrix();
	// Light source is directional, coming from the positive z-axis
	GLfloat lightPos[] = { 0.0, 0.0, 1.0, 0.0 };
	// Rotate light source so shadows appear on the bottom side of objects
	glRotatef( -45.0, 1.0, 0.0, 0.0 );
	glDisable( GL_LIGHTING );
	glLightfv( GL_LIGHT0, GL_POSITION, lightPos );
	glEnable( GL_LIGHTING );
	glPopMatrix();

	gBoard.Draw();
	gLauncher.Draw();

	glPopMatrix();

	glutPostRedisplay();
	glFlush();
	glutSwapBuffers();

	gLauncher.TimerCount();
}

void Reshape( int w, int h ){
	// Prevent divide by 0
	if( h == 0 ) h = 1;
	GLfloat ratio = (GLfloat)(w) / (GLfloat)(h);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glViewport( 0, 0, (GLsizei) w, (GLsizei) h );
	// Camera view projects out at 45 degree angle, objects visible from 50 units away to 150 units away from camera inside view frustum
	gluPerspective( 45.0, ratio, 1.0, 150.0 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void Mouse( int button, int state, int x, int y ){

}

void Keyboard( unsigned char key, int x, int y ){
	// Escape key always exits
	if( key == 27 )
		exit(0);

	// Only respond to game controls if a game is in session
	if( !gBoard.gameOver ){
		switch( key ){
			case ' ':
				if( gBoard.menu == 0 )
					gLauncher.Launch();
			//case 'v':
			//case 'V':
			//	gLauncher.PrintLists();
			default:
				if( gBoard.menu == 3 || gBoard.menu == 4 ){
					gBoard.gameOver = true;
					gBoard.menu = 0;
				}
		}
	}
	// Main menu
	else if( gBoard.menu == 0 ){
		switch( key ){
			case '1':
				gBoard.Initialize();
				break;
			case '2':
				gBoard.menu = 1;
				break;
			case '3':
				exit(0);
		}
	}
	// Controls menu
	else if( gBoard.menu == 1 ){
		switch( key ){
			default:
				gBoard.menu = 2;
		}
	}
	// Instructions menu
	else if( gBoard.menu == 2 ){
		switch( key ){
			default:
				gBoard.menu = 0;
		}
	}
}

void Arrowkeys( int key, int x, int y ){
	// Only respond to game controls if a game is in session
	if( !gBoard.gameOver && gBoard.menu == 0 ){
		switch( key ){
			case GLUT_KEY_LEFT:
				// Rotate aimer counterclockwise
				if( gLauncher.angleAim <= 160.0 )
					gLauncher.angleAim += 3.0;
				break;
			case GLUT_KEY_RIGHT:
				// Rotate aimer clockwise
				if( gLauncher.angleAim >= 20.0 )
					gLauncher.angleAim -= 3.0;
				break;
		}
	}
}