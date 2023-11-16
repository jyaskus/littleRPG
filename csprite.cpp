// CSprite class

#include "csprite.h"


int CCombatInfo::getOffense() // returns offensive value (attack)
{
	return offense; 
};

int CCombatInfo::getDefense() // returns defensive modifier (armor)
{
	return defense; 
};

void CCombatInfo::setHP(int newHP)
{
	health = newHP;	
};

int CCombatInfo::curHP()
{
	return health; 
};

void CCombatInfo::modHP(int mod)
{
	health += mod;
}

void CCombatInfo::setOffense(int newOff)
{
	offense = newOff;	
}
void CCombatInfo::setDefense(int newDef)
{
	defense = newDef;	
}
void CCombatInfo::modOff(int mod)
{
	offense += mod;	
}
void CCombatInfo::modDef(int mod)
{
	defense += mod;	
}

CCombatInfo::CCombatInfo(int Offense, int Defense, int Health) //, int AP) // constructor
{
	offense = Offense;
	defense = Defense;
	health  = Health;
};

void CUnitStats::setMaxHP(int newMax)
{
	maxHealth = newMax;
}
int CUnitStats::fullHP()
{
	return maxHealth;	
}
int CUnitStats::fullMana()
{
	return maxMana;	
}

// returns the OFFENSE for unit
int CUnitStats::getOff()
{
	return ((Str + Con + Lck)/3);	
}

// returns the DEFENSE for unit
int CUnitStats::getDef()
{
	return ((Dex + Int + Wis)/3);	
};

int CUnitStats::getValue(int Stat)
{
	switch(Stat)
	{
	case STRENGTH:     return Str; break;
	case DEXTERITY:    return Dex; break;
	case CONSTITUTION: return Con; break;
	case WISDOM:       return Wis; break;
	case INTELLIGENCE: return Int; break;
	case LUCK    :     return Lck; break;
	default : return -1; // unknown
	};
};
	
void CUnitStats::setStat(int Stat, int Value)
{
	switch(Stat)
	{
	case STRENGTH:     Str=Value; break;
	case DEXTERITY:    Dex=Value; break;
	case CONSTITUTION: Con=Value; break;
	case WISDOM:       Wis=Value; break;
	case INTELLIGENCE: Int=Value; break;
	case LUCK:         Lck=Value; break;
	};	
};

void CUnitStats::modStat(int Stat, int Change) // to permanently increase or decrease a statistic
{
	switch(Stat)
	{
	case STRENGTH:     Str+=Change; break;
	case DEXTERITY:    Dex+=Change; break;
	case CONSTITUTION: Con+=Change; break;
	case WISDOM:       Wis+=Change; break;
	case INTELLIGENCE: Int+=Change; break;
	case LUCK:         Lck+=Change; break;
	};	
};

CUnitStats::CUnitStats() // default constructor
{
	do
	{
	Str =  5 + rand() % 15; // 3 - 20
	Con =  6 + rand() % 15;
	Dex =  7 + rand() % 15;
	Wis =  8 + rand() % 15;
	Lck =  9 + rand() % 15;
	Int = 10 + rand() % 15;
	}
	while (Str+Dex+Con+Wis+Int+Lck <60); 
	// make sure they arent too low

	maxHealth = Str + Dex + Con; // phsyical stamina
	maxMana   = Wis + Int + Lck; // magical power
};


	// will assign default value based on random numbers
CUnitStats::CUnitStats(int s, int d, int c, int w, int i, int l) // constructor
{
	Str = s;
	Dex = d;
	Con = c;
	Wis = w;
	Int = i;
	Lck = l;
	maxHealth = Str + Dex + Con; // phsyical stamina
	maxMana   = Wis + Int + Lck; // magical power
};

void CSprite::Heal()
{
	Combat->setHP(Stats->fullHP());
}
int CSprite::curMana()		// returns current mana
{
	return mana;
}
bool CSprite::modMana(int modifier)
{
	if (mana + modifier < 0)
		return false;
	//else

	mana += modifier;

	if ( mana > Stats->fullMana() )
		mana = Stats->fullMana();

	return true;
}

void CSprite::setHP(int newHP)
{
	Combat->setHP(newHP);
	Stats->setMaxHP(newHP);
};

// returns 0 if still alive, 1 if dying/dead
bool CSprite::doDamage(int damage)
{
	Combat->modHP(-damage);

	if (Combat->curHP() < 1)
	{
		Combat->setHP(0); // no negative numbers
		STATE = DYING;
		return true;
	}
	// else
	return false;
};

int CSprite::nextFrame()
{ 
	if (++animcount > animdelay)
		animcount=0;
	else
		return 0; // use anim delay to slow down animations

	// increments current frame and checks for max
	if (++curframe > lastframe)
	{
		curframe=1;
		return 1;
	}
	else
		return 0;
}
int CSprite::getFrame()
{
	return curframe; 
}
void CSprite::setFrame(int newFrame)
{
	curframe=newFrame; 
}
void CSprite::setLastFrame(int newLastFrame)
{
	lastframe = newLastFrame; 
}
int CSprite::getLastFrame()
{
	return lastframe; 
}

void CSprite::setDelay(int newDelay)
{
	animdelay = newDelay; 
}
int CSprite::getDelay()
{
	return animdelay;
}

void CSprite::setupAnim(int numFrames, DIRS newDir, int perRow)
{
	curframe = 1;
	lastframe = numFrames-1;
	frames_per_anim = numFrames;
	facingDir = newDir;
	animdelay = 2;
	animcount = 0;
    columns = perRow; // sprite per row in sprite sheet
}

void CSprite::setXY(int newX, int newY)
{
	x = newX; 
	y = newY; 
}	

void CSprite::setMoveXY(int moveX, int moveY)
{ 
	movex = moveX;
	movey = moveY;
}

int CSprite::getX()
{
	return x; 
}
int CSprite::getY()
{
	return y; 
}

int CSprite::getMoveY()
{
	return movey; 
}
void CSprite::setMoveY(int newY)
{
	movey = newY; 
}

int CSprite::getMoveX()
{
	return movex; 
}
void CSprite::setMoveX(int newX)
{
	movex = newX;	
}

void CSprite::setX(int newX)
{
	x = newX;	
}
void CSprite::addX(int addtoX)
{
	x += addtoX; 
}

void CSprite::setY(int newY)
{
	y = newY;	
}
void CSprite::addY(int addtoY)
{
	y += addtoY; 
}

int CSprite::getHeight()
{
	return height; 
}
int CSprite::getWidth()
{
	return width;	
}

void CSprite::doMove(bool SAFE = true)
{
 //move the sprite
	x += movex;
	y += movey;

	if (SAFE)
	{
		// stop at edge, dont wrap
		if (x > border.right) // RIGHT
		{
			x=border.right;
			movex=0;
			movey=0;
			moving=false;
		}
		else if (x < border.left) // LEFT
		{
			x=border.left;
			movex=0;
			movey=0;
			moving=false;
		}

	   if (y > border.bottom) // BOTTOM
	   {
			y= border.bottom;
			movex=0;
			movey=0;
			moving=false;
	   }
	   else if (y < border.top) // TOP
		{
			y=border.top;
			movex=0;
			movey=0;
			moving=false;
		}
	}
}

LPDIRECT3DTEXTURE9 CSprite::getImage()
{
	return _image;
}

// returns a rect pointing to the sprites frame 
RECT CSprite::getRect()
{
	/*	EAST	= 1,
		NORTH	= 2,
		NE		= 3,
		NW		= 4,
		SOUTH	= 5,
		SE		= 6,
		SW		= 7,
		WEST	= 8		*/
	int offset; // offset starting location based on direction sprite is facing
	offset = (facingDir-1) * frames_per_anim; // 

	RECT srcRect;

	// will return the RECT point to current sprite, within sprite sheet		
	srcRect.left   = ((curframe+offset) % columns) * width;
	srcRect.top    = ((curframe+offset) / columns) * height;
	srcRect.right  = srcRect.left + width;
	srcRect.bottom = srcRect.top  + height;
	
	return srcRect;
}

DIRS CSprite::getFacing()
{
	return facingDir; 
}

void CSprite::setFacing(DIRS newFacing, bool changeMoveXY)
{
	facingDir = newFacing; 

	if (changeMoveXY)
		switch(newFacing)
		{
			case  NORTH:	movey=-2;				break;	// NORTH
			case  SOUTH:	movey= 2;				break;	// SOUTH
			case  EAST:		movex= 2;				break;	// EAST
			case  WEST:		movex=-2;				break;	// WEST
			case  NE:		movex= 2;	movey=-1;	break;	// NE
			case  NW:		movex=-2;	movey=-1;	break;	// NW
			case  SE:		movex= 2;	movey= 1;	break;	// SE
			case  SW:		movex=-2;	movey= 1;	break;	// SW
		};
} 

// check movement keys
void CSprite::checkArrowsKeys()
{
   if (KEY_DOWN(VK_UP) || KEY_DOWN(VK_NUMPAD8))
	{// move NORTH		
		movey=-2;
		movex=0;
		facingDir=NORTH;
		moving=true;				
	}
	else if (KEY_DOWN(VK_RIGHT) || KEY_DOWN(VK_NUMPAD6))	
	{
		moving=true;
		movex=2;
		movey=0;
		facingDir=EAST;

	}
	else if (KEY_DOWN(VK_DOWN) || KEY_DOWN(VK_NUMPAD2)) 
	{
		movey=2;
		movex=0;
		facingDir=SOUTH;
		moving=true;
	}
	else if (KEY_DOWN(VK_LEFT) || KEY_DOWN(VK_NUMPAD4))
	{
		movey=0;
		movex=-2;
		facingDir=WEST;
		moving=true;
	}
	else if (KEY_DOWN(VK_NUMPAD5))
	{
		// stop
		movex=0;
		movey=0;
		moving=false;
	}
	else if (KEY_DOWN(VK_NUMPAD9)) // NE
	{
		movex=2;
		movey=-1;
		facingDir=NE;
		moving=true;
	}
	else if (KEY_DOWN(VK_NUMPAD7)) // NW
	{
		movex=-2;
		movey=-1;
		facingDir=NW;
		moving=true;
	}
	else if (KEY_DOWN(VK_NUMPAD3)) // SE
	{
		movex=2;
		movey=1;
		facingDir=SE;
		moving=true;
	}
	else if (KEY_DOWN(VK_NUMPAD1)) // SW
	{
		movex=-2;
		movey=1;
		facingDir=SW;
		moving=true;
	};
	lastDir = facingDir;
} // end arrow key checks

bool CSprite::isMoving()
{
	return moving; 
}

void CSprite::setMoving(bool newState)
{
	moving = newState;	
}

void CSprite::setRhombus()
{
	// RHOMBUSHEIGHT
	// this will return the "iso diamond" at the bottom of the sprite
	int divisor = 2;

	// X
	int center = x + (width /2);
	rhombus.left  = center - (RHOMBUSWIDTH/divisor);
	rhombus.right = center + (RHOMBUSWIDTH/divisor)+1;

	center = (y + height) - (RHOMBUSHEIGHT*1.25);
	rhombus.top    = center - (RHOMBUSHEIGHT/divisor);
	rhombus.bottom = center + (RHOMBUSHEIGHT/divisor)+1;
}

RECT CSprite::getRhombus()
{
	// RHOMBUSHEIGHT
	// this will return the "iso diamond" at the bottom of the sprite
	// 32x32 RECT now - works easiery and quicker
	int divisor = 2;

	// X
	int center = x + (width /2);
	rhombus.left  = center - (RHOMBUSWIDTH/divisor);
	rhombus.right = center + (RHOMBUSWIDTH/divisor)+1;

	center = (y + height) - (RHOMBUSHEIGHT);//
	rhombus.top    = center - (RHOMBUSHEIGHT/divisor);
	rhombus.bottom = center + (RHOMBUSHEIGHT/divisor)+1;

	return rhombus;
}

POINT CSprite::getScreenPT()
{
	POINT tPT;
	tPT.x = x;
	tPT.y = y;

	return tPT;
}

void CSprite::setNewDIR(int newDir)
{
	switch(newDir)
	{
		case  1: movey=-2;				facingDir = NORTH;	break;	// NORTH
		case  2: movey= 2;				facingDir = SOUTH;	break;	// SOUTH
		case  3: movex= 2;				facingDir = EAST;	break;	// EAST
		case  4: movex=-2;				facingDir = WEST;	break;	// WEST
		case  5: movex= 2;	movey=-1;	facingDir = NE;		break;	// NE
		case  6: movex=-2;	movey=-1;	facingDir = NW;		break;	// NW
		case  7: movex= 2;	movey= 1;	facingDir = SE;		break;	// SE
		case  8: movex=-2;	movey= 1;	facingDir = SW;		break;	// SW
	};
}

// defines the screen walkable/collision border for this sprite
void CSprite::setBorder(int left, int right, int top, int bottom)
{
	border.left		= left;
	border.right	= right;
	border.top		= top;
	border.bottom	= bottom;
}

// returns TRUE if sprite is within its defined border, FALSE otherwise
bool CSprite::hitBorder()
{
		// see if it is inside the border boundaries
	return ( (x > border.right) || (x < border.left) || (y > border.bottom) || (y < border.top) );
}

void CSprite::setSTATE(STATEtype newS)
{
	STATE = newS;
}

int CSprite::getSTATE()
{
//	INACTIVE	= 0,
//	ACTIVE		= 1,		// moving
//	CASTING		= 2,
//	ATTACKING	= 3,
//	DYING		= 6
//	STATEtype STATE;
	return STATE;
}

POINT CSprite::getAnchor()
{
	POINT tPT;

	// for a 96x96 sprite, this is {45,64}
	tPT.x = x + width/2;
	tPT.y = y + height - 32;

	return tPT;
}

// constructor for new sprites ... curFrame will always be (1) .. scale and rotation not currently implemented
// and this is for non-animated sprites
CSprite::CSprite(int X, int Y, int Width, int Height, char *Filename)
{
	x = X;
	y = Y;
	width = Width;
	height = Height;

	// initialize these, but not currently in use since this is non-animated sprite
	curframe = lastframe = 1;
	animdelay = 3;
	animcount = 0;
	movex = movey = 0;
	facingDir = EAST; // default
	setRhombus();

	// Units stats
	Stats	= new CUnitStats();
	
	// assign full HP and generate offense/defense
	Combat	= new CCombatInfo(Stats->getOff(),Stats->getDef(),Stats->fullHP());
	
	// Sets STATS (Str,Dex,Con,Wis,Int,Lck) and SKILLS Offense/Defense/Mana/Health

	// set intial
	mana = Stats->fullMana();
	
	// Load the sprite file
	LoadImage(Filename);		
}

// destructor -- the default one should release all memory allocated for this sprite
 CSprite::~CSprite(void)
{
	// release the sprite sheet
	_image->Release();	
	
} 


void CSprite::LoadImage(char *Filename)
{
    _image = LoadTexture(Filename, D3DCOLOR_XRGB(255,0,255));
    assert(_image);        
}