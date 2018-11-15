// William Elmes
// Elmes_Randy@yahoo.com
// Bust a Move
// Interactive game based on the arcade game "Bust a Move"

#include "Launcher.h"

extern GameBoard gBoard;

#define PI 3.14159265358979323846264338327950288

Launcher::Launcher(){// Bottom center, reset point after ammoCurr's coords are changed
	baseX = 0.0;
	baseY = ( float(gBoard.H) * 0.5 + 1.5 ) * rowSpacing * (-1.0);
	// Bottom center
	currX = baseX;
	currY = baseY;
	// Bottom right corner
	nextX = float(gBoard.W) * 0.5 - 0.25;
	nextY = baseY;
}

Launcher::~Launcher(){
	while( !popCheck.empty() )
		popCheck.pop_front();
	while( !pop.empty() )
		pop.pop_front();
	while( !fallCheck.empty() )
		fallCheck.pop_front();
	while( !fall.empty() )
		fall.pop_front();
	while( !safe.empty() )
		safe.pop_front();
	while( !groupCheck.empty() )
		groupCheck.pop_front();
	while( !group.empty() )
		group.pop_front();
}

void Launcher::Initialize(){
	angleAim    = 90.0;
	timerLaunch = 0.0;
	loaded = true;

	// Initialize ammo
	ammoNext.Colorize();
	LoadNext();
}

void Launcher::LoadNext(){
	// Move up ammoNext to ammoCurr
	ammoCurr.color = ammoNext.color;
	// Reload ammoNext
	ammoNext.Colorize();
}

void Launcher::Launch(){
	// Only fire if Launcher is ready
	if( loaded ){
		// Unready Launcher and reset Launch variables
		loaded       = false;
		hasBounced   = false;
		launchPopped = 0;
		timerLaunch  = 0.0;
		angleLaunch  = angleAim;
	}
}

void Launcher::Draw(){
	// Don't draw if the game is over or has not yet started
	if( gBoard.gameOver )
		return;

	if( !loaded ){
		// If ammoCurr hits a wall, bounce off of it
		Ricochet();
		// Land ammoCurr when it hits the roof or a filled Bubble
		if( AtLanding() ){
			// Reload Launcher
			loaded = true;
			timerLaunch = 0.0;
			hasBounced = false;
			currX = baseX;
			currY = baseY;
			LoadNext();
			if( launchPopped == 0 )
				gBoard.RaiseGauge();
			else{
				while( launchPopped > 0 ){
					launchPopped--;
					gBoard.LowerGauge();
				}
			}
		}
	}
	// Draw ammoCurr and ammoNext
	ammoCurr.DrawProjectile( angleLaunch, timerLaunch, currX, currY );
	ammoNext.DrawProjectile( 0.0, 0.0, nextX, nextY );

	// Draw 4 cones pointing away from the bubble-to-be-fired in the direction of the launcher's angle
	// Move to Launcher's screen coordinates and rotate to match angleAim's direction (adjusted to face upwards)
	glPushMatrix();
	glColor4f( 1.0, 1.0, 1.0, 1.0 );
	glTranslatef( baseX, baseY, 0.0 );
	glRotatef( angleAim - 90.0, 0.0, 0.0, 1.0 );
	// Start the first cone an extra 0.4 units outward from Launcher's center
	glTranslatef( 0.0, 0.4, 0.0 );
	for( int i = 0; i < 4; i++ ){
		// Move out from Launcher's center, rotate to point cone outward, and draw cone
		glTranslatef( 0.0, 0.3, 0.0 );
		glPushMatrix();
		glRotatef( -90.0, 1.0, 0.0, 0.0 );
		glutSolidCone( 0.1, 0.15, 32, 16 );
		glPopMatrix();
	}
	// Done drawing aimer cones
	glPopMatrix();

	// Draw gauge meter
	glPushMatrix();
	glColor4f( 1.0, 1.0, 1.0, 1.0 );
	glLineWidth( 1.0 );
	// Bottom left corner
	glTranslatef( -nextX - 0.5, nextY, 0.0 );
	// Draw a box around the gauge meter
	glBegin( GL_LINE_LOOP );
	glVertex3f( 0.10,  0.15, 0.0 );
	glVertex3f( 0.10, -0.15, 0.0 );
	glVertex3f( float(gBoard.GAUGE_MAX) * 0.25 - 0.10, -0.15, 0.0 );
	glVertex3f( float(gBoard.GAUGE_MAX) * 0.25 - 0.10,  0.15, 0.0 );
	glEnd();
	// For each level of the gauge, draw a cube to the right of the last
	for( int i = 0; i < gBoard.GAUGE_MAX; i++ ){
		if( i < gBoard.gaugeLevel ){
			glTranslatef( 0.25, 0.0, 0.0 );
			glBegin( GL_QUADS );
			// Draw a square
			glVertex3f( -0.1,  0.1,  0.0 );
			glVertex3f( -0.1, -0.1,  0.0 );
			glVertex3f(  0.1, -0.1,  0.0 );
			glVertex3f(  0.1,  0.1,  0.0 );
			glEnd();
		}
	}
	// Done drawing gauge meter
	glPopMatrix();

	// Draw the roof
	// Find the screen coords for the current roof level
	float roofY = ( float(gBoard.roofLevel) - 1.5 - float(gBoard.H) * 0.5 ) * rowSpacing * (-1.0);
	glPushMatrix();
	glColor4f( 0.7, 0.7, 0.7, 1.0 );
	glBegin( GL_QUADS );
	// Draw a rectangle starting at the top of the screen, extending down to the roof level
	glVertex3f( -nextX - 0.55, -nextY + 0.55, 0.0 );
	glVertex3f( -nextX - 0.55,  roofY + 0.50, 0.0 );
	glVertex3f(  nextX + 0.55,  roofY + 0.50, 0.0 );
	glVertex3f(  nextX + 0.55, -nextY + 0.55, 0.0 );
	glEnd();
	// Done drawing the roof
	glPopMatrix();

	// Draw the lose threshhold line
	float loseY = ( float(gBoard.H) * 0.5 - 0.90 ) * rowSpacing * (-1.0),
		  gap   = -nextX - 0.55;
	glPushMatrix();
	glColor4f( 0.5, 0.5, 0.5, 0.1 );
	glLineWidth( 1.0 );
	glBegin( GL_LINES );
	while( gap < nextX + 0.50 ){
		// Draw a dashed line that indicates the level at which if any Bubble goes below, you lose
		glVertex3f( gap, loseY, 0.0 );
		gap += ( nextX * 2.0 + 1.1 ) * 0.01;
		glVertex3f( gap, loseY, 0.0 );
		gap += ( nextX * 2.0 + 1.1 ) * 0.01;
	}
	glEnd();
	// Done drawing lose threshhold line
	glPopMatrix();

	// Draw some borders
	glPushMatrix();
	glColor4f( 1.0, 1.0, 1.0, 1.0 );
	glLineWidth( 2.0 );
	glBegin( GL_LINE_LOOP );
	glVertex3f(  nextX + 0.55, -nextY + 0.55, 0.0 );
	glVertex3f( -nextX - 0.55, -nextY + 0.55, 0.0 );
	glVertex3f( -nextX - 0.55,  nextY - 0.55, 0.0 );
	glVertex3f(  nextX + 0.55,  nextY - 0.55, 0.0 );
	glEnd();
	// Done drawing borders
	glPopMatrix();
}

void Launcher::Ricochet(){
	// gap is the distance from the Launcher to a wall (half the total width of the board)
	float deltaX, gap = float(gBoard.W) * 0.5 - 0.25;
	// If the projectile has already bounced, it's next bounce will be from wall to wall instead of center to wall: twice the distance
	if( hasBounced )
		gap = float(gBoard.W) - 0.5;

	// Change in x = cos(angle in degrees) * distance
	deltaX = cos( angleLaunch * PI / 180 ) * timerLaunch;
	// If change in x meets or exceeds the distance to the wall the projectile is traveling towards, bounce it
	if( deltaX >= gap || deltaX <= gap * (-1.0) ){
		// Bounce the Bubble
		hasBounced = true;
		// Modify the curr coords to be the impact point
		currX += cos( angleLaunch * PI / 180.0 ) * timerLaunch;
		currY += sin( angleLaunch * PI / 180.0 ) * timerLaunch;
		// Reflect the angle off the wall
		angleLaunch = 180.0 - angleLaunch;
		// Reset travel distance from the new curr coords
		timerLaunch = 0.0;
	}
}

bool Launcher::AtLanding(){
	float screenX, screenY, fieldC, fieldR;
	int C, R;
	// Calculate ammoCurr's current x,y screen coords
	screenX = currX + cos( angleLaunch * PI / 180.0 ) * timerLaunch;
	screenY = currY + sin( angleLaunch * PI / 180.0 ) * timerLaunch;
	// Calculate ammoCurr's current c,r field indices (will NOT be whole number!)
	fieldR  = (-1.0) * screenY / rowSpacing - float(gBoard.roofLevel) + 2.5 + float(gBoard.H) * 0.5;
	fieldC  = screenX - ( int( floor( fieldR + 1.5001 )) % 2 ) * 0.5 + float(gBoard.W) * 0.5 + 0.75;
	// Round to nearest whole number to find useable field indices
	// Note: field indices start at 1 instead of 0, so we have to add 1 to our original equation
	C = int( floor( fieldC + 0.5001 ));
	R = int( floor( fieldR + 0.5001 ));
	// Because rows are staggered, column index may be rounded out of range, need to prevent this;
	if( C > gBoard.W )
		C = gBoard.W;
	if( C < 1 )
		C = 1;

	// ammoCurr has not yet reached the top of the grid, can't be at a landing spot yet
	if( R > gBoard.H + 2 )
		return false;

	// Check the field Bubble with the corresponding indices to see if it is touching any filled Bubbles on the grid and if its trajectory indicates a collision
	if( gBoard.field[C-1][R-1].TouchingFilled() && gBoard.field[C-1][R-1].DetectCollision( fieldC, fieldR, angleLaunch ) ){
		gBoard.field[C-1][R-1].color = ammoCurr.color;
		// Now that we know ammoCurr is at a landing spot touching a filled Bubble, check if it's touching a matching Bubble
		if( gBoard.field[C-1][R-1].TouchingMatching() ){
			Bubble* ptr = &gBoard.field[C-1][R-1];
			popCheck.push_back( ptr );
			ChainPop();
		}
		// Check if the last fired Bubble landed beyond the losing threshhold
		for( int i = 0; i < gBoard.W; i++ ){
			if( !gBoard.field[i][gBoard.H - gBoard.roofLevel + 1].Empty() ){
				gBoard.AchieveDefeat();
				break;
			}
		}
		return true;
	}

	// Check if ammoCurr has reached the roof of the grid, but not touching any filled Bubbles
	else if( gBoard.field[C-1][R-1].TouchingRoof() ){
		gBoard.field[C-1][R-1].color = ammoCurr.color;
		return true;
	}

	// Within range of grid, not touching roof, not touching filled Bubble
	return false;
}

void Launcher::ChainPop(){
	Bubble *check, *adjacent;
	list<Bubble*>::iterator adjPtr;
	int popSize = 0;
	bool bubbleRemaining = false;
	// Chain check every Bubble touching and matching the color of the launched and landed Bubble and each other
	while( !popCheck.empty() ){
		check = popCheck.front();
		adjPtr = check->adjacents.begin();
		while( adjPtr != check->adjacents.end() ){
			adjacent = *adjPtr;
			// If the bubble matches the current Bubble being check, and is not in queue to be chained,
			// and has not already been checked, add it to the queue to be chained
			if( check->color == adjacent->color && !adjacent->IsIn( popCheck ) && !adjacent->IsIn( pop ) ){
				popCheck.push_back( adjacent );
			}
			adjPtr++;
		}
		// Flag current Bubble as checked and move on to next Bubble to be checked
		pop.push_back( check );
		popCheck.pop_front();
		// Count the number of Bubbles in the current matching-color group
		popSize++;
		/*cout << "Matching group size: " << popSize << endl;*/
	}
	// Only pop the matching-color group if it's big enough
	if( popSize >= POP_SIZE_REQUIREMENT ){
		PopMatching();
		// Check top most row for any remaining Bubbles after last pop sequence
		for( int i = 0; i < gBoard.W; i++ ){
			if( !gBoard.field[i][0].Empty() ){
				// Found at least one unpopped Bubble
				bubbleRemaining = true;
				break;
			}
		}
		// If no filled Bubbles were found in top row, there can't be any left, so you win
		if( !bubbleRemaining )
			AchieveVictory();
	}
	else{
		// Don't need pop this time, flush it
		while( !pop.empty() ){
			pop.pop_front();
		}
	}
}

void Launcher::PopMatching(){
	Bubble *check, *adjacent;
	list<Bubble*>::iterator adjPtr;
	// Chain pop all matching Bubbles that touched the fired Bubble
	while( !pop.empty() ){
		check = pop.front();
		check->Pop();
		adjPtr = check->adjacents.begin();
		// Check for neighbors who might fall without the last popped Bubble
		while( adjPtr != check->adjacents.end() ){
			adjacent = *adjPtr;
			// Must be filled, not already in queue, and not in queue to be popped to be put into queue
			// to be check to see if the Bubble was left floating from the matching Bubbles being popped
			if( !adjacent->Empty() && !adjacent->IsIn( fallCheck ) && !adjacent->IsIn( pop ) )
				fallCheck.push_back( adjacent );
			adjPtr++;
		}
		pop.pop_front();
	}
	ChainFall();
}

// This was the hardest part of code!
void Launcher::ChainFall(){
	Bubble *checkF, *checkG, *adj;
	list<Bubble*>::iterator adjPtr;
	bool touchingRoof;
	// For each Bubble adjacent to a Bubble that was just popped, check if it was left floating and needs to be dropped
	while( !fallCheck.empty() ){
		/*cout << "Starting fall check...\n";*/
		// Consider this group of Bubbles to be floating until proven at least one member is touching the roof
		touchingRoof = false;
		checkF = fallCheck.front();
		fallCheck.pop_front();
		// If already in safe, it was already check in another iteration, don't check the same Bubble twice
		if( !checkF->IsIn( safe ) ){
			groupCheck.push_back( checkF );
			// ChainFall takes one Bubble from fallCheck and puts it into groupCheck. Here, we do a breadth first search to chain that Bubble's group
			// That is, we find all Bubbles that are directly touching this Bubble or indirectly touching through some number of filled Bubbles touching one another
			while( !groupCheck.empty() ){
				/*cout << "Starting group check...\n";*/
				checkG = groupCheck.front();
				groupCheck.pop_front();
				group.push_back( checkG );
				// For every Bubble adjacent to this one that is filled and has not been checked and is not waiting to be checked, add it to the list to be checked
				adjPtr = checkG->adjacents.begin();
				while( adjPtr != checkG->adjacents.end() ){
					/*cout << "Starting adjacent check...\n";*/
					adj = *adjPtr;
					if( !adj->Empty() && !adj->IsIn( group ) && !adj->IsIn( groupCheck ) ){
						groupCheck.push_back( adj );
					}
					adjPtr++;
				}
				if( checkG->TouchingRoof() ){
					// This Bubble is touching the roof, so its entire group is safe from being dropped
					touchingRoof = true;
				}
			}
			// groupCheck is now empty, so we've finished chaining this group
			// Here, at least one Bubble was touching the roof, so the entire group is safe from being dropped
			// Add all Bubbles in this group to the safe list so we don't check their group again this round when we check the next member of fallCheck, and flush the group list
			if( touchingRoof ){
				/*cout << "Found a safe group!\n";*/
				while( !group.empty() ){
					checkG = group.front();
					group.pop_front();
					safe.push_back( checkG );
				}
			}
			// No Bubble in this group was touching the roof, so it's been left floating after the matching color Bubbles were popped
			// Drop all Bubbles in this group and flush the group list
			else{
				/*cout << "Found a floating group!\n";*/
				while( !group.empty() ){
					checkG = group.front();
					group.pop_front();
					checkG->Pop();
					launchPopped++;
				}
			}
		}
	}
	// We're done check for floating Bubbles now, flush safe list so next launch can use it
	while( !safe.empty() )
		safe.pop_front();
}

void Launcher::AchieveVictory(){
	gBoard.menu = 4;
}

void Launcher::TimerCount(){
	// Used as travel distance for ammoCurr, only increase while ammoCurr is traveling
	if( !loaded ){
		// Scale speed with the GameBoard's height
		timerLaunch += 0.015 * float(gBoard.H);
	}
}

void Launcher::PrintLists(){
	list<Bubble*>::iterator ptr;
	int count;

	// Simply output the number of elements in each list; testing purposes
	count = 0;
	cout << "popCheck: ";
	ptr = popCheck.begin();
	while( ptr != popCheck.end() ){
		ptr++;
		count++;
	}
	cout << count << endl;
	count = 0;
	cout << "pop: ";
	ptr = pop.begin();
	while( ptr != pop.end() ){
		ptr++;
		count++;
	}
	cout << count << endl;
	count = 0;
	cout << "fallCheck: ";
	ptr = fallCheck.begin();
	while( ptr != fallCheck.end() ){
		ptr++;
		count++;
	}
	cout << count << endl;
	count = 0;
	cout << "fall: ";
	ptr = fall.begin();
	while( ptr != fall.end() ){
		ptr++;
		count++;
	}
	cout << count << endl;
	count = 0;
	cout << "safe: ";
	ptr = safe.begin();
	while( ptr != safe.end() ){
		ptr++;
		count++;
	}
	cout << count << endl;
	count = 0;
	cout << "groupCheck: ";
	ptr = groupCheck.begin();
	while( ptr != groupCheck.end() ){
		ptr++;
		count++;
	}
	cout << count << endl;
	count = 0;
	cout << "group: ";
	ptr = group.begin();
	while( ptr != group.end() ){
		ptr++;
		count++;
	}
	cout << count << endl;
}