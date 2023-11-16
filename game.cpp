// Beginning Game Programming, 2nd Edition
// Chapter 8
// Tiled_Sprite program source code file

#include <cassert>
#include "game.h"

// default background map
int MAPDATA[MAPWIDTH*MAPHEIGHT];

// used for displaying the sprites
LPD3DXSPRITE sprite_handler;

// used to prevent stacking overlays
int mapOverlay[MAPWIDTH][MAPHEIGHT];

// holds the info on the map overlays
OverlayType MapOverlays[MAX_OVERLAYS];

int tempX,tempY; // used as a temp, to store current location

int level = 0; // starting level

//the wave sound
CSound *sound_fire,*sound_exp, *sound_sword, *sound_club,*sound_magic,*sound_walk_left,*sound_walk_right,*sound_miss, *sound_orc_hit, *sound_orc_grunt, *sound_heal;

// UNITs
CSprite *player,*player_casting,*player_dying;

// enemy #1
CSprite *orc,*orc_attacking,*orc_dying;

// enemy #2
CSprite *mushy, *mushy_attacking, *mushy_dying, *mushy_appearing;

// overlay tiles
LPDIRECT3DTEXTURE9 tiles,bushes;

// MAGIC
CSprite *firebolt,*explosion;
//RECT magic_border;

// used to print numbers on screen
CSprite *numbers, *bars;

// text graphics
LPDIRECT3DSURFACE9 text_destroyed,text_orcs,text_bushes,text_level,text_header;

// counter for trees destroyed so far
int trees_destroyed		=	0;
int enemy_destroyed		=	0;
int reset_death			=	0;

// timer for recovery of mana
int mana_timer			=	0;
SPELLS curSpell			=	SPELL_NONE;

// track which overlay # fireball collided with last, -1 means none
int fireball_collision = -1;

//timing variable
long start = GetTickCount();
HRESULT result;

// ----------------------- GAME FUNCTIONS ----------------------- //

//initializes the game
int Game_Init(HWND hwnd)
{
	//set random number seed
	srand(time(NULL));

   //initialize mouse
    if (!Init_Mouse(hwnd))
    {
        MessageBox(hwnd, "Error initializing the mouse", "Error", MB_OK);
        return 0;
    }

    //initialize keyboard
    if (!Init_Keyboard(hwnd))
    {
        MessageBox(hwnd, "Error initializing the keyboard", "Error", MB_OK);
        return 0;
    }

    //create sprite handler object
    result = D3DXCreateSprite(d3ddev, &sprite_handler);
    if (result != D3D_OK)
	 return 0;

	// load map data
	ifstream mapFile("map00.txt");

	// read in data from map file
	assert(! mapFile.fail());

	// int MAPDATA[MAPWIDTH*MAPHEIGHT]
	//MAPWIDTH   16  
	//MAPHEIGHT  45 
	char buffer[256];
    char line[255];
	int row=0;
	while (! mapFile.eof())
	{ // read in the map data
		for (int column=0; column < (MAPWIDTH-1); column++)
		{
			mapFile.getline(line, sizeof(buffer), ',');
			MAPDATA[row * MAPWIDTH + column] = atoi(line);			
		}
		// grab the end of line
		mapFile.getline(line, sizeof(buffer), '\n');
        MAPDATA[row * MAPWIDTH + (MAPWIDTH-1)] = atoi(line);

        row++;
	}
	mapFile.close();


    /*
	NOTE: loading textures with "pink" (255,0,255) as the transparent color 
	*/

	// health mana bars
	bars = new CSprite(100,SCREEN_HEIGHT-64,64,6,"bars.bmp");
	assert(bars);
	bars->setupAnim(1,EAST,7);

	//sprites for tile background
	tiles = LoadTexture("tiles.bmp", D3DCOLOR_XRGB(255,0,255));
	assert(tiles);

	// sprites for overlay (over original tiles)
	bushes = LoadTexture("bushes.bmp", D3DCOLOR_XRGB(255,0,255));
	assert(bushes);

	// Menu stuff
	text_destroyed	= LoadSurface("text_destroyed.bmp", D3DCOLOR_XRGB(0,0,0));
	assert(text_destroyed);
	text_orcs	= LoadSurface("text_orcs.bmp", D3DCOLOR_XRGB(0,0,0));
	assert(text_orcs);
	text_bushes = LoadSurface("text_bushes.bmp", D3DCOLOR_XRGB(0,0,0));
	assert(text_bushes);
	text_level	= LoadSurface("text_level.bmp", D3DCOLOR_XRGB(0,0,0));
	assert(text_level);
	text_header = LoadSurface("border.bmp", D3DCOLOR_XRGB(0,0,0));
	assert(text_header);


	// numbers
	numbers = new CSprite(0,0,18,16,"little_numbers.bmp");
	assert(numbers);
	numbers->setupAnim(10,EAST,10); // 1 row, numbers 0 to 9

//player
	player = new CSprite(80,80,96,96,"stan-staff-walking.bmp");
	assert(player); 
	player->setupAnim(9,SOUTH,10); // 9 sprites per anim, initially heading EAST, 10 sprites per row on sheet
	player->setMoveXY(0,0);
	player->setMoving(false);
	player->setBorder(8,SCREEN_WIDTH - 96 - 16,-16,SCREEN_HEIGHT - 96 -32); // works for 96x96 sprites, keeps sprite from walking off screen
	player->setSTATE(ACTIVE);

	// casting a spell
	player_casting = new CSprite(0,0,96,96,"stan-staff-magic.bmp");
	assert(player_casting);
	player_casting->setupAnim(11,EAST,10); //  sprites per anim, initial heading ,  sprites per row on sheet
	player_casting->setMoveXY(0,0);
	player_casting->setMoving(false);	

	// dying
	player_dying = new CSprite(0,0,96,96,"stan-staff-killed.bmp");
	assert(player_dying);
	player_dying->setupAnim(9,EAST,10); //  sprites per anim, initial heading ,  sprites per row on sheet
	player_dying->setMoveXY(0,0);
	player_dying->setMoving(false);	


// MONSTER(s)
	//"orc" walking, attacking, dying
	int destX = SCREEN_WIDTH  - 128;
	int destY = SCREEN_HEIGHT - 128;

	orc = new CSprite(destX,destY,96,96,"orc.bmp");
	assert(orc);
	orc->setBorder(8,SCREEN_WIDTH - 112,-16,SCREEN_HEIGHT - 128); // works for 96x96 sprites, keeps sprite from walking off screen
	orc->setupAnim(8,NW,10); // sprites per anim, initial heading, sprites per row on sheet	
	orc->setSTATE(ACTIVE);
	orc->Combat->setDefense(10);
	orc->Combat->setOffense(12);
	orc->setHP(22);

	orc_attacking = new CSprite(destX,destY,96,96,"orc-attacking.bmp");
	assert(orc_attacking);
	orc_attacking->setupAnim(13,SOUTH,10); // sprites per anim, initial heading, sprites per row on sheet
	orc_attacking->setMoveXY(0,0);

	orc_dying = new CSprite(destX,destY,128,128,"orc-dying.bmp");
	assert(orc_dying);
	orc_dying->setupAnim(13,SOUTH,8); // sprites per anim, initial heading, sprites per row on sheet
	orc_dying->setMoveXY(0,0);

	// MUSHROOM MAN
	mushy = new CSprite(destX,destY,96,96,"mushman_walk.bmp");
	assert(mushy);
	mushy->setBorder(8,SCREEN_WIDTH - 112,-16,SCREEN_HEIGHT - 128); // works for 96x96 sprites, keeps sprite from walking off screen
	mushy->setupAnim(8,EAST,10); // sprites per anim, initial heading, sprites per row on sheet	
	mushy->setSTATE(INACTIVE); // doesnt start visible
	mushy->Combat->setDefense(10);
	mushy->Combat->setOffense(8);
	mushy->setHP(12);

	mushy_attacking = new CSprite(destX,destY,96,96,"mushman_attack.bmp");
	assert(mushy_attacking);
	mushy_attacking->setupAnim(11,EAST,10); // sprites per anim, initial heading, sprites per row on sheet
	mushy_attacking->setMoveXY(0,0);

	mushy_dying = new CSprite(destX,destY,96,96,"mushman_dying.bmp");
	assert(mushy_dying);
	mushy_dying->setupAnim(7,EAST,10); // sprites per anim, initial heading, sprites per row on sheet
	mushy_dying->setMoveXY(0,0);

	mushy_appearing = new CSprite(destX,destY,96,96,"mushman_appear.bmp");
	assert(mushy_appearing);
	mushy_appearing->setupAnim(7,EAST,10); // sprites per anim, initial heading, sprites per row on sheet
	mushy_appearing->setMoveXY(0,0);

//fire bolt 
    firebolt = new CSprite(SCREEN_WIDTH,SCREEN_HEIGHT,64,64,"firebolt.bmp");
	assert(firebolt); 
	firebolt->setupAnim(21,EAST,16); // 21 sprites per anim, initially heading EAST, 16 sprites per row on sheet
	firebolt->setDelay(2);
	// used to check when it is close to edge of map; it explodes if so 
	firebolt->setBorder(32,SCREEN_WIDTH - 64,8,SCREEN_HEIGHT - 48); 
	firebolt->setSTATE(INACTIVE);

// + explosion
    explosion = new CSprite(SCREEN_WIDTH,SCREEN_HEIGHT,75,78,"explosion2.bmp");//explosion.bmp 75x78 - 15 per anim, 5 per row
	assert(explosion); 
	explosion->setupAnim(15,EAST,5); // 15 sprites per anim, initially heading EAST, 5 sprites per row on sheet
	explosion->setSTATE(INACTIVE);

	// Load the Sound effects / wav files
    //firebolt launched
    sound_fire = LoadSound("fire.wav");
    assert(sound_fire);

    //explosion
    sound_exp = LoadSound("explosion-01.wav"); 
    assert(sound_exp);

    //attack - sword
    sound_sword = LoadSound("attack sword.wav"); 
    assert(sound_sword);

    //attack - club/staff
    sound_club = LoadSound("attack club.wav"); 
    assert(sound_club);

    //magic
    sound_magic = LoadSound("magic.wav"); 
    assert(sound_magic);

    //magic heal
    sound_heal = LoadSound("magic 2.wav"); 
    assert(sound_heal);

    //attack miss
    sound_miss = LoadSound("klink.wav"); 
    assert(sound_miss);

    //orc hit/dying
    sound_orc_hit = LoadSound("orc-death.wav"); 
    assert(sound_orc_hit);

    //orc grunt
    sound_orc_grunt = LoadSound("orc grunt.wav"); 
    assert(sound_orc_grunt);

	/* ----------- SETUP the overlays  -------- */
	// initialize map overlay array to (0) 
	// to make sure there are no duplicate location used
	for (int y=0; y < MAPHEIGHT; y++)
		for (int x = 0; x < MAPWIDTH; x++)
			mapOverlay[x][y] = 0;

	// default is (50) random overlays
	int omin = 1;
	int omax = NUM_OVERLAYS; // number of overlay tiles in the file
	int randTile;
	int tileX,tileY;
	POINT tPT;

	// MAX_OVERLAYS controls # of overlays to use
	for (int i=0; i < MAX_OVERLAYS; i++) 
	{
		randTile = rand() % omax + omin;

		do
		{		
			// keep away from map edges
			tileX = rand() % (MAPWIDTH  -4) + 2; 
			tileY = rand() % (MAPHEIGHT -4) + 2; 
		} while (mapOverlay[tileX][tileY] != 0);
		// repeat until xy chosen is equal to 0, which means no overlay
		
		// now assign the random tile to chosen xy
		mapOverlay[tileX][tileY] = randTile;

		// determine on screen xy from tile xy
		tPT = tile_to_world(tileX,tileY);

		// overlay details
		MapOverlays[i].tile		= randTile;		// overlay tile #
		MapOverlays[i].x		= tileX;		// tile x
		MapOverlays[i].y		= tileY;		// tile y

		// overlay rectangle for collision checking (screen xy)
		MapOverlays[i].r1.left   = tPT.x + 15;
		MapOverlays[i].r1.right  = tPT.x + 47; // 32 pixels wide (x)
		MapOverlays[i].r1.top    = tPT.y + 24;
		MapOverlays[i].r1.bottom = tPT.y + 48; // 24 pixels tall (y)
	}	

    //return okay
    return 1;
}



//the main game loop
void Game_Run(HWND hwnd)
{
    //make sure the Direct3D device is valid
    if (d3ddev == NULL)
        return;

	//update mouse and keyboard
    Poll_Mouse();
    Poll_Keyboard();

    //after short delay, ready for next frame?
    //this keeps the game running at a steady frame rate
    if (GetTickCount() - start >= 30)
    {
		// recover some mana, slowly
		mana_timer++;
		if (mana_timer >= MANA_DELAY)
		{
			player->modMana(1);
			mana_timer = 0;
		}

        //reset timing
        start = GetTickCount();

	   // CHECK for SPACEBAR
	   if ( KEY_DOWN(VK_SPACE) && (firebolt->getSTATE() == INACTIVE)  && (explosion->getSTATE() == INACTIVE) && (player->getSTATE() == ACTIVE))	
	   {
		   // returns true if have enough, false if not
		   if (! player->modMana(- MANA_FIRE) )
		   {
		   	   PlaySound(sound_miss);
		   }
		   else
		   { // enough mana to cast it, lets do it!
			   player->setSTATE(CASTING);
			   curSpell = SPELL_FIRE;

			   //casting_spell = true;
			   player_casting->setFrame(0);// it will be incremented (1) before displayed
			   player_casting->setLastFrame(9);
			   player->setMoveXY(0,0);
			   player->setMoving(false);
			   player_casting->setXY(player->getX(),player->getY()); // set XY
			   player_casting->setFacing(player->getFacing(),false);          // make sure player is facing correct direction

			   // spell casting sound
			   PlaySound(sound_magic);
		   }
	   };

	   // CHECK for NUM PAD 0 = HEAL
	   if ( KEY_DOWN(VK_NUMPAD0) && (player->getSTATE() == ACTIVE))	
	   {
		   // returns true if have enough, false if not
		   if (! player->modMana(- MANA_HEAL) )
		   {
		   	   PlaySound(sound_miss);
		   }
		   else
		   { // enough mana to cast it, lets do it!
			   player->setSTATE(CASTING);

			   curSpell = SPELL_HEAL;

			   //casting_spell = true;
			   player_casting->setFrame(0);// it will be incremented (1) before displayed
			   player_casting->setLastFrame(9);
			   player->setMoveXY(0,0);
			   player->setMoving(false);
			   player_casting->setXY(player->getX(),player->getY()); // set XY
			   player_casting->setFacing(player->getFacing(),false);          // make sure player is facing correct direction

			   // spell casting sound
			   PlaySound(sound_magic);
		   }
	   };

	   // check status of player avatar
	   switch(player->getSTATE())
	   {
		case INACTIVE: break; 
		case ACTIVE:
			{
				// respond to arrow keys and move player unit
				player->checkArrowsKeys();
				player->doMove(true); 

				// check for collision, player vs. overlays
				// if found, reverse movement and stop player
				if ( collisionOverlays(player->getRhombus()) )
				{
					// ADD: 
					// if close to a small mushroom - make it destroy the overlay
					// and spawn a mushroom

					// cancel move
					tempX = player->getX() - player->getMoveX();
					tempY = player->getY() - player->getMoveY();
					player->setXY(tempX,tempY);

					// STOP moving
					player->setMoveXY(0,0);

					// reset anim - if moved
					if (player->getFrame() > 1)
						player->setFrame(1); // reset to initial frame of animation also

					// make sure player is now stopped
					player->setMoving(false);
				}

				//animate player sprite -- if moving and not casting a spell
				if (player->isMoving())
					player->nextFrame();
			};
			break;
		case CASTING:
			{
			   if ( player_casting->nextFrame() > 0) // increment frame ... if its reached the end,it will return > 0
				   if (curSpell == SPELL_FIRE)
				   {			   
					   // not active until spell casting anim is done
					   player->setSTATE(ACTIVE);
					   firebolt->setSTATE(ACTIVE);

					   // setup the firebolt sprite			   
					   firebolt->setFacing(player->getFacing(),false);			   
					   tempX=tempY=0;
					   switch(player->getFacing())
					   {
						case NORTH: tempY=-4;	break;
						case SOUTH: tempY=4;	break;
						case  EAST: tempX= 4;	break;
						case  WEST: tempX=-4;	break;
						case    NE: tempX=4;	
									tempY=-2;	break;
						case    NW: tempX=-4;	
									tempY=-2;	break;
						case    SE: tempX=4;	
									tempY=2;	break;
						case    SW: tempX=-4;	
									tempY=2;	break;
					   };
					   firebolt->setMoveXY(tempX,tempY);

					   // set X and Y to center of player sprite
					   switch(player->getFacing())
					   {
						case NORTH: tempX=24;
									tempY=-16;  break;
						case SOUTH: tempX=16;
									tempY=64;	break;
						case  EAST: tempY=16;
									tempX=64;	break;
						case  WEST: tempY=8;
									tempX=-16;	break;
						case    NE: tempX=32;	
									tempY=-8;	break;
						case    NW: tempX=-8;	
									tempY=-8;	break;
						case    SE: tempX=48;	
									tempY=48;	break;
						case    SW: tempX=-8;	
									tempY=48;	break;
					   };
					   firebolt->setXY(player->getX()+tempX,player->getY()+tempY);

					   // firebolt sound
					   PlaySound(sound_fire);
				   }  // ... casting a spell and reach last frame - which activates the explosion
				   else
					   if (curSpell == SPELL_HEAL)
					   {
						   // does not take effect until spell casting anim is done
						   player->setSTATE(ACTIVE);

						   PlaySound(sound_heal);

						   player->Heal(); // full heal
					   }
			};	
			break;
		case DYING: 
			{
				// advance frame - see if its done
				if ( player_dying->nextFrame() > 0)
				{
					player->setSTATE(DEAD);				
					player_dying->setFrame(player_dying->getLastFrame());
					player->setDelay(0);
				}
			}
			break;
		case DEAD:
			{
				// delay for a few seconds
				player->setDelay(player->getDelay() + 2);

				// you are dead, game over
				// waits a few cycles first though
				if (player->getDelay() > DEATH_DELAY)
					PostMessage(hwnd, WM_DESTROY, 0, 0);
			}; 
			break;
	   }

		// if explosion is already happening - continue animation
	    if (explosion->getSTATE() == ACTIVE)
		{				
			// animate sprite
			if (explosion->nextFrame() > 0)
			{
			   //explosion_active = false; // left the screen (x)
				explosion->setSTATE(INACTIVE);

			   // "destroy" the overlay so it disappears
			   if (fireball_collision > -1)
			   {
				   if ((MapOverlays[fireball_collision].tile >= 10) && (MapOverlays[fireball_collision].tile <= 18))					   
				   	   if (mushy->getSTATE() == INACTIVE)
					   {
						   // set the new state
						   mushy->setSTATE(APPEARING);						   

						   // set the startin location, offsetting for sprite size difference						    
						   mushy->setX(MapOverlays[fireball_collision].r1.left -31); // offset for difference in size
						   mushy->setY(MapOverlays[fireball_collision].r1.top  -40);
						   mushy_appearing->setX(MapOverlays[fireball_collision].r1.left -31); // offset for difference in size
						   mushy_appearing->setY(MapOverlays[fireball_collision].r1.top  -40);
					   }

				   // set collided overlay to (-1) so its ignored
				   MapOverlays[fireball_collision].r1.left   = -1;
				   MapOverlays[fireball_collision].r1.top    = -1;
				   MapOverlays[fireball_collision].r1.right  = -1;
				   MapOverlays[fireball_collision].r1.bottom = -1;
				   MapOverlays[fireball_collision].tile =  -1;
				   fireball_collision = -1;							// reset
			   }

			   // fireball_collision ctonains [i] of map over lay which was collided with
			}
			else
			{
				if (explosion->getFrame() == 8)
					if (fireball_collision > -1) 
					   {
						   // this should make the overlay dis-appear
						   // before the fireball explosion is done
						   MapOverlays[fireball_collision].x = -1;
						   MapOverlays[fireball_collision].y = -1;

						   trees_destroyed++; // increment counter for trees destroyed
					   }			
			}

		}		

		// animate the firebolt
		//if (fireball_active)
		if (firebolt->getSTATE() == ACTIVE)
		{	
			// grab current location
			tempX = firebolt->getX();
			tempY = firebolt->getY();

			//move the sprite & animate it
			firebolt->doMove(false);			
			firebolt->nextFrame();

			// CHECK COLLISION	
			// OVERLAYS & SPRITE
			fireball_collision =  getOverlayCollided(firebolt->getRhombus());
			if ( fireball_collision > -1 )
			{
				// fireball collided
				firebolt->setMoveXY(0,0); // stop
				firebolt->setSTATE(INACTIVE);
			}

			// see if firebolt hits the enemy sprite
			if (orc->getSTATE() != INACTIVE)
				if ( rectCollision(orc->getRhombus(),firebolt->getRhombus()) )
				{ 
					// Orc was hit by fireball - apply damage
					// fireball collided
					firebolt->setMoveXY(0,0); // stop
					//fireball_active=false; // it explodes, so vanish
					firebolt->setSTATE(INACTIVE);
					fireball_collision=-1;

					// apply damage
					int OFF = player->Combat->getOffense() + rollDice(6,3); // +3d6 for sword/firebolt
					int DEF = orc->Combat->getDefense();					

					// Apply damage
					if ((OFF - DEF) < 1)
					{
						// target missed
						PlaySound(sound_miss);
					}
					else
					{   // target hit
						// play sound - orc dying
						PlaySound(sound_orc_hit);

						//apply the damage and see if still alive
						if ( orc->doDamage(OFF - DEF) ) // true=dying false=still alive
						{
							// orc is now dying
							orc->setSTATE(DYING);
							orc->setMoving(false);
							orc->setMoveXY(0,0);
							
							// set position of orc-dying
							orc_dying->setXY(orc->getX()-16,orc->getY()-16);
						}
					} // applying damage
				} // fireball collides with orc ...

			// see if firebolt hits the enemy sprite
			if (mushy->getSTATE() != INACTIVE)
				if ( rectCollision(mushy->getRhombus(),firebolt->getRhombus()) )
				{ 
					// Orc was hit by fireball - apply damage
					// fireball collided
					firebolt->setMoveXY(0,0); // stop
					//fireball_active=false; // it explodes, so vanish
					firebolt->setSTATE(INACTIVE);
					fireball_collision=-1;

					// apply damage
					int OFF = player->Combat->getOffense() + rollDice(6,3); // +3d6 for sword/firebolt
					int DEF = mushy->Combat->getDefense();					

					// Apply damage
					if ((OFF - DEF) < 1)
					{
						// target missed
						PlaySound(sound_miss);
					}
					else
					{   // target hit
						// play sound - mushy dying
						PlaySound(sound_orc_hit);

						//apply the damage and see if still alive
						if ( mushy->doDamage(OFF - DEF) ) // true=dying false=still alive
						{
							// mushy is now dying
							mushy->setSTATE(DYING);
							mushy->setMoving(false);
							mushy->setMoveXY(0,0);
							
							// set position of mushy-dying
							mushy_dying->setXY(mushy->getX(),mushy->getY());
							// set its facing as well
							mushy_dying->setFacing(mushy->getFacing(),false);
						}
					} // applying damage
				} // fireball collides with mushy ...

			// check if the fireball hits a border
			if ( firebolt->hitBorder() )
			   firebolt->setSTATE(INACTIVE); 			   

			// if fireball went inactive, explode it
			if (firebolt->getSTATE() == INACTIVE)
			{
				//explosion_active = true;
				explosion->setSTATE(ACTIVE);

				tempX = firebolt->getX(); 
				tempY = firebolt->getY(); 
				switch(firebolt->getFacing())
				{
				case EAST: tempX += 24; tempY -=16; break;
				case NE:   tempX +=  8; tempY -=16; break;
				case SE:   tempX -= 16; tempY -=16; break;
				case NORTH:tempY -=32;
					break;
				case SOUTH:tempY += 8;
					break;
				case SW: tempX -= 16; tempY -=24;break;
				case NW: tempX -= 16; tempY -=16;break;
				case WEST: tempX -= 16; tempY -= 16;break;
				}
				// make it explode close to contact location

				// use previous location of firebolt to explode, centered
				explosion->setXY(tempX,tempY); 				

				// BOOOMM!
				PlaySound(sound_exp);
			}
		}; // ... if fireball active 

		// check status of ORC
		switch(orc->getSTATE())
		{
			case INACTIVE: { 
							// do nothing
						   } break;
			case ACTIVE:   { 
							// see if he is close enough to attack
							if (findDistance(orc->getAnchor(), player->getAnchor()) <= 25.f) // pixels
							{
								// make sure player isnt dead
								if ( player->getSTATE() > CASTING )
									break;

								// ATTACK
								player->setMoveXY(0,0);
								player->setMoving(false);
								orc->setMoveXY(0,0);

								// set anim state
								orc->setSTATE(ATTACKING);

								// set location on screen
								orc_attacking->setXY(orc->getX(),orc->getY());

								// turn to face player
								orc_attacking->setFacing( getDirection(player->getAnchor(), orc_attacking->getAnchor()),false);

								// exit from this switch statement
								break;
							}

							// animate and make sounds
							orc->nextFrame();

						   // see if player is "in sight"
						   // try to head towards player
						   if (  findDistance(orc->getAnchor(), player->getAnchor()) <= SIGHT_RANGE )
							   if (rand() % 50 > 25)
								   {   // 50% chance of turning towards player
									   // possible direction towards Player
									   DIRS temp = getDirection(player->getScreenPT(), orc->getScreenPT());

									   // set new direction and facing
									   orc->setFacing(temp,true);
								   }  

							// check if not moving, move random
							if ( (orc->getMoveX() == 0) && (orc->getMoveY() == 0) )
							{			
							   int oldDir = orc->getFacing();

							   tempX=tempY=0;
							   int newDir;							   
							   do
							   {
								   newDir = rand() % 8 + 1; // random direction
							   } while(newDir == oldDir);

							   // set new direction and facing
							   orc->setNewDIR(newDir);
							}

							// move
							orc->doMove(true);

							// check for collision, sprite vs. overlays
							if ( collisionOverlays(orc->getRhombus()) )
							{
								// cancel move
								tempX = orc->getX() - orc->getMoveX();
								tempY = orc->getY() - orc->getMoveY();
								orc->setXY(tempX,tempY);

								// STOP moving
								orc->setMoveXY(0,0);

								// reset anim - if moved
								if (orc->getFrame() > 1)
									orc->setFrame(1); // reset to initial frame of animation also

								// make sure orc is now stopped
								orc->setMoving(false);
							}
						   } break;
			case ATTACKING:  
						   { 
							// make sure player isnt dead
							if ( player->getSTATE() > CASTING )
							{
								// attack over
								orc->setSTATE(ACTIVE);
								break;
							}

							// see if the attack is done
							if (orc_attacking->nextFrame() > 0)
							{
								// attack finished
								orc->setSTATE(ACTIVE);

								// see if target is still "in range"
								if (findDistance(orc->getAnchor(), player->getAnchor()) > 25.f)
									break; // break out if target is gone now
								
								// just a test of sound
							    PlaySound(sound_sword);

								// apply damage
								int OFF = orc->Combat->getOffense() + rollDice(6,3); // + 3d6
								int DEF = player->Combat->getDefense();

								// Apply damage
								if ((OFF - DEF) > 0)
								{
									if (player->doDamage(OFF - DEF) > 0)
									{
										// player is now dying ...
										player->setSTATE(DYING);
									}
								}
								else 
									PlaySound(sound_miss);
									// its a miss .. play sounds
							}
					       } break;
			case DYING:    { 
								orc_dying->nextFrame(); // advance frame
								if ( orc_dying->getFrame() == orc_dying->getLastFrame() )
								{
									orc->setSTATE(DEAD);
									enemy_destroyed++;
									orc->setDelay(0);
								}
					       } break;

			case DEAD:		{
								orc->setDelay(orc->getDelay() + 1);
								if (orc->getDelay() > DEATH_DELAY)
								{
									// orc grunts as he gets up
									PlaySound(sound_orc_grunt);

									// wake up orc
									orc->Heal();
									orc->setSTATE(ACTIVE);
									orc->setFrame(1);
									orc->setDelay(3); // default delay
								}
							}
							break; 
		} // end ORC state check

		switch(mushy->getSTATE())
		{
			case INACTIVE: { 
							// do nothing
						   } break;
			case ACTIVE:   { 
							// see if he is close enough to attack
							if (findDistance(mushy->getAnchor(), player->getAnchor()) <= 25.f) // pixels
							{
								// make sure player isnt dead
								if ( player->getSTATE() > CASTING )
									break;

								// ATTACK
								player->setMoveXY(0,0);
								player->setMoving(false);
								mushy->setMoveXY(0,0);

								// set anim state
								mushy->setSTATE(ATTACKING);

								// set location on screen
								mushy_attacking->setXY(mushy->getX(),mushy->getY());

								// turn to face player
								mushy_attacking->setFacing( getDirection(player->getAnchor(), mushy_attacking->getAnchor()) ,false);
								mushy_attacking->setFrame(1);

								// exit from this switch statement
								break;
							}

							// animate and make sounds
							mushy->nextFrame();

							// check if not moving, move random
						   // see if player is "in sight"
						   // try to head towards player
							if (  findDistance(mushy->getAnchor(), player->getAnchor()) <= SIGHT_RANGE )
								if (rand() % 50 > 25)
								   {
									   // possible direction towards Player
									   DIRS temp = getDirection( player->getScreenPT(),mushy->getScreenPT() );

									   if (temp != mushy->getFacing())
										   mushy->setFacing(temp,true);
								   }

							if ( (mushy->getMoveX() == 0) && (mushy->getMoveY() == 0) )
							{ 
							   int newDir;						   
							   do
							   {
								   newDir= rand() % 8 + 1; // random direction
							   } while(newDir == mushy->getFacing());
							   // make sure its not the same move as last time
							   // so he doesnt repeat same behavior 
							   mushy->setNewDIR(newDir);
							}

							// move
							mushy->doMove(true);

							// check for collision, sprite vs. overlays
							if ( collisionOverlays(mushy->getRhombus()) )
							{
								// cancel move
								tempX = mushy->getX() - mushy->getMoveX();
								tempY = mushy->getY() - mushy->getMoveY();
								mushy->setXY(tempX,tempY);

								// STOP moving
								mushy->setMoveXY(0,0);

								// reset anim - if moved
								if (mushy->getFrame() > 1)
									mushy->setFrame(1); // reset to initial frame of animation also

								// make sure mushy is now stopped
								mushy->setMoving(false);
							}
						   } break;
			case ATTACKING:  
						   { 
							// make sure player isnt dead
							if ( player->getSTATE() == DEAD )
							{	// attack over
								mushy->setSTATE(ACTIVE);
								break;
							}

							// increment frame and 
							// see if the attack is done
							if (mushy_attacking->nextFrame() > 0)
							{
								// attack finished
								mushy->setSTATE(ACTIVE);

								// see if target is still "in range"
								if (findDistance(mushy->getAnchor(), player->getAnchor()) > 25.f)
									break; // break out if target is gone now
								
								// just a test of sound
							    PlaySound(sound_club);

								// apply damage
								int OFF = mushy->Combat->getOffense() + rollDice(6,3); // + 3d6
								int DEF = player->Combat->getDefense();

								// Apply damage
								if ((OFF - DEF) > 0)
								{
									if (player->doDamage(OFF - DEF) > 0)
									{
										// player is now dying ...
										player->setSTATE(DYING);
									}
								}
								else 
									PlaySound(sound_miss);
									// its a miss .. play sounds
							}
					       } break;
			case APPEARING: { 
								// Spawns in the mushroom man, from a tiny shroom
								if ( mushy_appearing->nextFrame() > 0)
								{
									mushy->setSTATE(ACTIVE);
									mushy->setXY(mushy_appearing->getX(),mushy_appearing->getY());
									mushy->setFacing(mushy_appearing->getFacing(),false);
								}
						   } break;

			case DYING:    { 
								// advance frame
								if (mushy_dying->nextFrame() > 0)								
								{
									mushy_dying->setFrame(mushy_dying->getLastFrame());
									mushy->setSTATE(DEAD);
									mushy->setDelay(0);
									enemy_destroyed++;

									// magic mushrooms, full hp & mana
									player->Heal();
									player->modMana(50); // caps out at max anyways
									PlaySound(sound_heal);
								}
					       } break;
			case DEAD:	   {
								// corpse to stick around for a short while
								mushy->setDelay(mushy->getDelay() +1);

								if (mushy->getDelay() > DEATH_DELAY)
								{	// set it up for "next time"
									mushy->Heal();
									mushy->setSTATE(INACTIVE);
									mushy->setFrame(1);
									mushy->setDelay(3); // default delay
								}
						   }
						   break; 
		} // end MUSHMAN state check

   }
   
   /* ----- DRAW SCREEN ----- */

   //     if (GetTickCount() - start >= 30)

    //start rendering
    if (d3ddev->BeginScene())
    {
        //erase the entire background
        d3ddev->ColorFill(backbuffer, NULL, D3DCOLOR_XRGB(0,0,0));

        //start sprite handler
        sprite_handler->Begin(D3DXSPRITE_ALPHABLEND);

		// draw tiles to back buffer -- using textures so it uses transparency properly
		DrawTilesBack();

		// draw the sprites ontop of the tiles
		DrawSprites();

		// special effects go last 
		if (explosion->getSTATE() == ACTIVE)
		{	
			//create vector to update sprite position
			D3DXVECTOR3 position((float)explosion->getX(), (float)explosion->getY(), 0);
			//draw the explosion sprite
			sprite_handler->Draw(explosion->getImage(), &explosion->getRect(),NULL,	&position,D3DCOLOR_XRGB(255,255,255));
		} // explosion

		// draw the game menu stuff
		drawMenu();

        //stop drawing
        sprite_handler->End();

		//stop rendering
		d3ddev->EndScene();
	} // if (d3ddev->BeginScene())

    //display the back buffer on the screen
    d3ddev->Present(NULL, NULL, NULL, NULL);

    //check for escape key (to exit program)
    if (KEY_DOWN(VK_ESCAPE))
        PostMessage(hwnd, WM_DESTROY, 0, 0);

}

//frees memory and cleans up before the game ends
void Game_End(HWND hwnd)
{
	Kill_Keyboard();
	Kill_Mouse();

	fflush(stdout); 
	fclose(stdout);

//	if (dinput != NULL)
//		dinput->Release();

    if (sprite_handler != NULL)
        sprite_handler->Release();
}
// END ----------------------- GAME FUNCTIONS ----------------------- //

// test for collisions
int SpriteCollision(CSprite *sprite1, CSprite *sprite2)
{
    RECT rect1;
    rect1.left   = sprite1->getX() + 1;
    rect1.top    = sprite1->getY() + 1;
    rect1.right  = sprite1->getX() + sprite1->getWidth()  -1;
    rect1.bottom = sprite1->getY() + sprite1->getHeight() -1;

    RECT rect2;
    rect2.left   = sprite2->getX() + 1;
    rect2.top    = sprite2->getY() + 1;
    rect2.right  = sprite2->getX() + sprite2->getWidth()  -1;
    rect2.bottom = sprite2->getY() + sprite2->getHeight() -1;

    RECT dest;
    return IntersectRect(&dest, &rect1, &rect2);
}

// test for collisions
bool rectCollision(RECT r1, RECT r2)
{

return ( r1.left   < r2.right  &&
		 r1.top    < r2.bottom &&
		 r1.right  > r2.left   &&		 
		 r1.bottom > r2.top    );
}

// POINT lastCollision;
// returns TRUE if sprite rhombus overlays and overlays
// FALSE otherwise -- and returns TRUE on first collision
bool collisionOverlays(RECT dest_rhombus)
{
 // CHECK COLLISION	// OVERLAYS & SPRITE

	// check if the given RECT overlaps with 
	for (int i=0; i<= MAX_OVERLAYS; i++)
		if ( rectCollision(dest_rhombus, MapOverlays[i].r1) )
			return true;						

	return false; // if no collisions found 
}

//returns overlay # which has collided
int getOverlayCollided(RECT dest_rhombus)
{
	// CHECK COLLISION	// OVERLAYS & SPRITE
	for (int i=0; i<= MAX_OVERLAYS; i++)
		// check if the given RECT overlaps with 
		if (rectCollision(dest_rhombus, MapOverlays[i].r1))
			return i;						

	return -1; // if no collisions found 
}


// returns a RECT for collisions
RECT getTileRECT(int x,int y) // screen xy
{
	RECT rhombus;

	// returns a rectangle pointing to rhombus of tile (based on the xy coords provided)
	int center = x + (TILEWIDTH /2); // 0 + 32
	rhombus.left  = center - (RHOMBUSWIDTH/2);
	rhombus.right = center + (RHOMBUSWIDTH/2);

	center = (y + TILEHEIGHT) - (RHOMBUSHEIGHT);
	rhombus.top    = center - (RHOMBUSHEIGHT/2);
	rhombus.bottom = center + (RHOMBUSHEIGHT/2);

	return rhombus;
}

//returns screen coordinates for a given x,y tile location
// which refers to where to draw the 64x64 sprite, so the bottom
// 64x32 rhombus lines up properly
POINT tile_to_world(int tilex, int tiley)
{
	POINT tPoint;

	tPoint.x = tilex * TILEWIDTH + (tiley & 1)*(TILEWIDTH/2);
	tPoint.y = tiley * (TILEHEIGHT/4);

	return tPoint;
}

//This function does the real work of drawing a single tile from the 
//source image onto the backbuffer. 
void DrawSprite(LPDIRECT3DTEXTURE9 source,	// source surface image
				int tilenum,				// tile #
				int width,					// tile width (although ALL sprites used this way are 64x64)
				int height,					// tile height
				int columns,				// columns of tiles
				int destx,					// destination x
				int desty)					// destination y
{
    //create a RECT to describe the source image
    RECT r1;
    r1.left = (tilenum % columns) * width;
    r1.top = (tilenum / columns) * height;
    r1.right = r1.left + width;
    r1.bottom = r1.top + height;

	// location to draw sprite
	D3DXVECTOR3 position((float)destx, (float)desty, 0);

	//draw the player sprite
	sprite_handler->Draw(
		source, 
		&r1,
		NULL,
		&position,
		D3DCOLOR_XRGB(255,255,255));
}

// draws the layer #2 (bushes) and #3 (player objects)
void DrawSprites()
{
    int tiles_row=3; // tiles per row in sprite sheet
    int columns, rows;
    int x, y;
	POINT destPT;

    //calculate the number of columns and rows
    columns = MAPWIDTH; 
    rows    = MAPHEIGHT;
    
    //draw tiles onto the scroll buffer surface
    for (y=0; y < rows; y++)
	{
		for (x=0; x < columns; x++)
		{			
			// convert from tile xy to screen xy
			destPT = tile_to_world(x,y);

			// LAYER #2 - TREES
			// cycle through overlays, to see if any share this tile x and y
			for (int i=0; i < MAX_OVERLAYS;i++)
				if ( (MapOverlays[i].x == x) && (MapOverlays[i].y == y) )
					{
						// bushes are all 64x64 ... so we can ignore any offsetX,Y values -- they will center OK
						DrawSprite(bushes, MapOverlays[i].tile ,TILEWIDTH,TILEHEIGHT,8,destPT.x,destPT.y);
					}

			// LAYER #3 - SPRITES		
			// player

			// using basic rect collision detection - to determine if player sprite overlaps this tile
			RECT tRect;
			tRect.top    = player->getY()+1;			
			tRect.left   = player->getX()+1;
			tRect.bottom = tRect.top + (player->getHeight()*0.6) -4 ; // allows player to walk behind upper part of overlays, like trees
			tRect.right  = tRect.left + player->getWidth() -4;
			// rectangle around players sprite -- to check if it overlaps background sprites
	/* SPRITES */
		// PLAYER
			// compares player sprite RECT with a given tiles RECT
			if ( rectCollision(tRect, getTileRECT(destPT.x,destPT.y)) )
				{
					//create vector to update sprite position
					D3DXVECTOR3 playerPos((float)player->getX(), (float)player->getY(), 0);

					//draw the player sprite
					switch(player->getSTATE())
					{
						case ACTIVE:
							{
								sprite_handler->Draw(player->getImage(), &player->getRect(),NULL,&playerPos, D3DCOLOR_XRGB(255,255,255));
							};
							break;
						case CASTING:
							{
								sprite_handler->Draw(player_casting->getImage(), &player_casting->getRect(),NULL,&playerPos, D3DCOLOR_XRGB(255,255,255));
							};
							break;
						case DYING:
							{
								sprite_handler->Draw(player_dying->getImage(), &player_dying->getRect(),NULL,&playerPos, D3DCOLOR_XRGB(255,255,255));
							};
							break;
						case DEAD:
							{
								sprite_handler->Draw(player_dying->getImage(), &player_dying->getRect(),NULL,&playerPos, D3DCOLOR_XRGB(255,255,255));
							};
							break;
					}
				} // if ...

		// ORC
			// if orc is active and might overlap these tiles
			tRect.top    = orc->getY()+1;			
			tRect.left   = orc->getX()+1;
			tRect.bottom = tRect.top + (orc->getHeight()*0.6) -4 ; // allows orc to walk behind upper part of overlays, like trees
			tRect.right  = tRect.left + orc->getWidth() -4;

			// If the sprites RECT might collide with this tile, draw it --
			if ( rectCollision(tRect, getTileRECT(destPT.x,destPT.y)) )
				switch(orc->getSTATE())
				{
					case INACTIVE: break; // do nothing
					case ACTIVE:   { 
									D3DXVECTOR3 position((float)orc->getX(), (float)orc->getY(), 0);
									sprite_handler->Draw(orc->getImage(), &orc->getRect(),NULL,&position, D3DCOLOR_XRGB(255,255,255));
								   } break;
					case ATTACKING:  
								   { 
									D3DXVECTOR3 position((float)orc_attacking->getX(), (float)orc_attacking->getY(), 0);
									sprite_handler->Draw(orc_attacking->getImage(), &orc_attacking->getRect(),NULL,&position, D3DCOLOR_XRGB(255,255,255));
								   } break;
					case DYING:    { 
									D3DXVECTOR3 position((float)orc_dying->getX(), (float)orc_dying->getY(), 0);
									sprite_handler->Draw(orc_dying->getImage(), &orc_dying->getRect(),NULL,&position, D3DCOLOR_XRGB(255,255,255));
								   } break;
					case DEAD:    { 
									//orc_dying->setFrame(13); // last frame
									D3DXVECTOR3 position((float)orc_dying->getX(), (float)orc_dying->getY(), 0);
									sprite_handler->Draw(orc_dying->getImage(), &orc_dying->getRect(),NULL,&position, D3DCOLOR_XRGB(255,255,255));
								   } break;
				}

			// if mushy is active and might overlap these tiles
			tRect.top    = mushy->getY()+1;			
			tRect.left   = mushy->getX()+1;
			tRect.bottom = tRect.top + (mushy->getHeight()*0.6) -4 ; // allows mushy to walk behind upper part of overlays, like trees
			tRect.right  = tRect.left + mushy->getWidth() -4;

			// If the sprites RECT might collide with this tile, draw it --
			if ( rectCollision(tRect, getTileRECT(destPT.x,destPT.y)) )
				switch(mushy->getSTATE())
				{
					case INACTIVE: break; // do nothing
					case ACTIVE:   { 
									D3DXVECTOR3 position((float)mushy->getX(), (float)mushy->getY(), 0);
									sprite_handler->Draw(mushy->getImage(), &mushy->getRect(),NULL,&position, D3DCOLOR_XRGB(255,255,255));
								   } break;
					case ATTACKING:  
								   { 
									D3DXVECTOR3 position((float)mushy_attacking->getX(), (float)mushy_attacking->getY(), 0);
									sprite_handler->Draw(mushy_attacking->getImage(), &mushy_attacking->getRect(),NULL,&position, D3DCOLOR_XRGB(255,255,255));
								   } break;
					case DYING:    { 
									D3DXVECTOR3 position((float)mushy_dying->getX(), (float)mushy_dying->getY(), 0);
									sprite_handler->Draw(mushy_dying->getImage(), &mushy_dying->getRect(),NULL,&position, D3DCOLOR_XRGB(255,255,255));
								   } break;
					case APPEARING:{ 
									D3DXVECTOR3 position((float)mushy_appearing->getX(), (float)mushy_appearing->getY(), 0);
									sprite_handler->Draw(mushy_appearing->getImage(), &mushy_appearing->getRect(),NULL,&position, D3DCOLOR_XRGB(255,255,255));
								   } break;
					case DEAD:    { 
									D3DXVECTOR3 position((float)mushy_dying->getX(), (float)mushy_dying->getY(), 0);
									sprite_handler->Draw(mushy_dying->getImage(), &mushy_dying->getRect(),NULL,&position, D3DCOLOR_XRGB(255,255,255));
								   } break;
				}


		// FIREBALL
			// create basic collision rect
			tRect.top    = firebolt->getY()+1;			
			tRect.left   = firebolt->getX()+1;
			tRect.bottom = tRect.top + (firebolt->getHeight()*0.6) -4 ;
			tRect.right  = tRect.left + firebolt->getWidth() -4;

			// if fireball is active and might overlap these tiles
			if (firebolt->getSTATE() == ACTIVE)
				if ( rectCollision(tRect, getTileRECT(destPT.x,destPT.y)) )
					{
						//create vector to update sprite position
						D3DXVECTOR3 position3((float)firebolt->getX(), (float)firebolt->getY(), 0);

						//draw the firebolt sprite
						sprite_handler->Draw(firebolt->getImage(), &firebolt->getRect(),NULL,&position3, D3DCOLOR_XRGB(255,255,255));
					} // if ...
		} // for x ...
	} // for y ...
}

//This function fills the tilebuffer with tiles representing
//the current scroll display based on scrollx/scrolly.
// draws straight to backbuffer
void DrawTilesBack()
{
    int tiles_per_row=BACK_PER_ROW; // tiles per row in sprite sheet
    int columns, rows;
    int x, y; // destX,destY;
    int tilenum;
	POINT destPT;
    
    //calculate the number of columns and rows
    columns = MAPWIDTH;  //WINDOWWIDTH  / TILEWIDTH;      // 64 * 20 = 1280
    rows    = MAPHEIGHT; //WINDOWHEIGHT / (TILEHEIGHT/4); // 32 * 24 = 768
    
    //draw tiles onto the scroll buffer surface
    for (y=0; y < rows; y++)
	{
		for (x=0; x < columns; x++)
		{
			// skip tile #16 on odd rows
			if ((x+1 == columns) && (y & 1))
			{
				// skip last tile on odd rows, so it fits on screen better
			}
			else
			{
				// offset for isometric maps on odd rows 
				destPT = tile_to_world(x, y);

				//retrieve the tile number from this position
				tilenum = MAPDATA[(y) * MAPWIDTH + (x)];

				// LAYER #1 - TILES
				//draw the tile onto the scroll buffer
				DrawSprite(tiles,tilenum,TILEWIDTH,TILEHEIGHT,tiles_per_row, destPT.x, destPT.y);
			}
		} // for x ...
	} // for y ...
} // drawtiles ... 

float findDistance(POINT pt1,POINT pt2)
{
	float distance;
	float	dx = pt1.x - pt2.x; 
	float	dy = pt1.y - pt2.y;
	distance = sqrt(dx*dx + dy*dy);
	return distance;
}

// roll dice
int rollDice(int Dice, int Number = 1)
{
	int sum=0;
	for (int i=1; i <= Number; i++)
		sum += rand() % Dice + 1;

	return sum;
};

// draw bar at location; using current, max # and color
void drawBar(int x, int y,int curr,int max, int color)
{
	// 0 for red, 1 for blue
	int width  = 64;
	int height = 6;
	RECT r1 =  {0,0,width,height};

	D3DXVECTOR3 position((float)x, (float)y, 0);

	//draw the sprite
	bars->setFrame(0);
	r1 = bars->getRect();
	sprite_handler->Draw(bars->getImage(), &r1,NULL,&position, D3DCOLOR_XRGB(255,255,255));

	// draw bar %
	// percentage
	int pixels	 = width * curr / max;

	// set frame for color
	bars->setFrame(color);

	r1 = bars->getRect();
	r1.right = r1.left + pixels;
	sprite_handler->Draw(bars->getImage(), &r1,NULL,&position, D3DCOLOR_XRGB(255,255,255));
}

// determine which direction to turn to face target
DIRS getDirection(POINT sourcePT, POINT targetPT)
{
	int a = 0; // centered horizontally
	// left to right
	if (sourcePT.x < targetPT.x) // left
		a = -1;
	if (sourcePT.x > targetPT.x) // right
		a = 1;

	int b = 0; // centered vertically
	// top to bottom
	if (sourcePT.y < targetPT.y) // above
		b = -1;
	if (sourcePT.y > targetPT.y) // below
		b = 1;

	switch(a)
	{
		case -1:
			if (b == -1)
				{ return NW; }
			else if (b == 1)
				{ return SW; }
			else
				return WEST;
			break;
		case 1:
			if (b == -1)
				{ return NE; }
			else if (b == 1)
				{ return SE; }
			else
				return EAST;
			break;
		case 0:
			if (b == -1)
				{ return NORTH; }
			else
//				if (b == 1)
				{ return SOUTH; }
			break;
	}


}

// draw numbers to screen
void drawNumbers(int number, int screenX, int screenY)
{
	int tile_width  = 18;
	int tile_height = 16;

	// no more than 10 digits I hope
	char buffer[99];// = itoa(number); //"1234";
	sprintf(buffer,"%d",number);

	int tempX=screenX;
	int tempY=screenY;
	int digit;
	int end = strlen(buffer);

	for (int cnt=0; cnt < end; cnt++)
	{		
		if (cnt >= 10)
			break;
		//create vector to update sprite position
		D3DXVECTOR3 position((float) tempX + (tile_width * cnt), (float)tempY, 0);

		char temp = buffer[cnt];
        digit = atoi(&temp);
		
		// draw the tile at location
		numbers->setFrame(digit);
		sprite_handler->Draw(numbers->getImage(), &numbers->getRect(),NULL,	&position,D3DCOLOR_XRGB(255,255,255));
	}
}

// draws all the words and numbers that make up the games menu system
void drawMenu()
{

	int x = 0;
	int y = 0;

	RECT r1;
	r1.left		= x;
	r1.right	= x + 707;
	r1.top		= y;
	r1.bottom	= y + 27;
	d3ddev->StretchRect(text_header, NULL, backbuffer, &r1, D3DTEXF_NONE);

	// Level
	x=60;
	drawNumbers(level,x,y);		

	// PLAYER info
	// current HP
	x=200;
	drawNumbers(player->Combat->curHP(),x,y);	
	if (player->getSTATE() != DEAD)
	{
		drawBar(16+player->getX(),player->getY(),player->Combat->curHP(),player->Stats->fullHP(),5);  // red bar
		drawBar(16+player->getX(),player->getY()+6,player->curMana(),player->Stats->fullMana(),1);    // blue bar
	}

    // PLAYER Offense
	x+=60;
	drawNumbers(player->Combat->getOffense(),x,y);		

	// PLAYER Defense
	x+=60;
	drawNumbers(player->Combat->getDefense(),x,y);	

	// --- SCORE ---
	// ORC info
	drawNumbers(enemy_destroyed,600,y);		
	// Bushes
	drawNumbers(trees_destroyed,720,y);		

	// players mana
//	drawNumbers(player->curMana(),800,y);		

	// -- Health Bars -- //
	// ORC
	if (orc->getSTATE() != DEAD)
		drawBar(16+orc->getX(),orc->getY(),orc->Combat->curHP(),orc->Stats->fullHP(),5);

	// MUSHROOM MAN
	if ((mushy->getSTATE() != DEAD) && (mushy->getSTATE() != INACTIVE))
		drawBar(16+mushy->getX(),mushy->getY(),mushy->Combat->curHP(),mushy->Stats->fullHP(),5);

};