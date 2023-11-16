// CSprite class header

#ifndef CSPRITE_H
#define CSPRITE_H

//windows/directx headers
#include <d3d9.h>
#include <d3dx9.h>
#include <assert.h>

//framework-specific headers
//#include "dxinput.h"
#include "dxgraphics.h"

//macros to read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

// diamond at bottom of tiles
#define RHOMBUSWIDTH  24 //32 // 64
#define RHOMBUSHEIGHT 24     
#define HEIGHTOVERLAPPING (RHOMBUSHEIGHT/2)+1

// unit logic
// range (in pixels) to check for player as target
#define SIGHT_RANGE 200

typedef enum
{ // matches the order of sprites in sprite sheets now //
	EAST	= 1,
	NORTH	= 2,
	NE		= 3,
	NW		= 4,
	SOUTH	= 5,
	SE		= 6,
	SW		= 7,
	WEST	= 8
} DIRS;

// these are used to define the unit states
typedef enum
{
	INACTIVE	= 0,
	ACTIVE		= 1,		// moving
	CASTING		= 2,
	ATTACKING	= 3,
	APPEARING	= 4,
	DYING		= 6,
	DEAD        = 9
} STATEtype;

typedef enum
{
	STRENGTH       = 1, // strength
	DEXTERITY      = 2, // dexterity
	CONSTITUTION   = 3,  // constitution
	WISDOM         = 4, // wisdom
	INTELLIGENCE   = 5, // intelligence
	LUCK           = 6  // luck
} STATISTIC;

class CCombatInfo
{
public:
	int getOffense(); // returns offensive value (attack)
	int getDefense(); // returns defensive modifier (armor)

	void setHP(int newHP);
	void modHP(int mod);
     int curHP();

	void setOffense(int newOff);
	void setDefense(int newDef);
	void modOff(int mod);
	void modDef(int mod);

	CCombatInfo(int Offense, int Defense, int Health); // constructor

private:
	int offense;
	int defense;
	int health; // current health
};


class CUnitStats // physical statistics of units
{
public:

	int fullHP();		// returns full health
	int fullMana();		// returns full mana 

	int getOff();		// returns offense skill
	int getDef();		// returns defense skill

	int getValue(int Stat);				// returns value of given statistic
	void setStat(int Stat, int Value);	// sets stat to given value
	void modStat(int Stat, int Change);	// to permanently increase or decrease a statistic
	void setMaxHP(int newMax);          // changes the max HP

	CUnitStats();	// default constructor, assigns random values to each stat					
	CUnitStats(int s, int d, int c, int w, int i, int l); // alternate constructor

private:
	int Str, Dex, Con, Wis, Int, Lck, maxHealth, maxMana;
};


// CUSTOM SPRITE CLASS //
class CSprite
{
public:	

	// Public - so their functions can be used outside
	CUnitStats  *Stats; // abilties and such
	CCombatInfo *Combat; // using random defaults

	// fully heals unit
	void Heal();
	// changes the units current and max HP value
	void setHP(int newHP); 
	// returns current mana
	int curMana();		
	// add/sub from mana pool ... returns false if not enough mana to do it
	bool modMana(int modifier);

	// applies damage to the unit
	// returns 0 if still alive, 1 if dying/dead
	bool doDamage(int damage);

	 int nextFrame();				 // increments frame and returns 1 if its reached the end
	 int getFrame();				 // returns current frame
	void setFrame(int newFrame);	 // sets current frame to new value

	void setLastFrame(int newLastFrame); // sets the last frame of animation sequence
	int getLastFrame();					 // returns current "last" frame

	void setDelay(int newDelay);		// sets the delay used for animations
 	 int getDelay();					// returns current value

	void setupAnim(int numFrames, DIRS newDir, int perRow);

	void setXY(int newX, int newY);
     int getX();
	 int getY();
	void setX(int newX);
	void setY(int newY);
	void addX(int addtoX);
	void addY(int addtoY);

	void setMoveXY(int moveX, int moveY);
	void setMoveX(int newX);
	void setMoveY(int newY);
	 int getMoveY();
	 int getMoveX();

	 int getHeight();
	 int getWidth();

	// check movement keys
	void checkArrowsKeys();
	void doMove(bool SAFE);

	// holds the units sprite sheet
	LPDIRECT3DTEXTURE9 getImage();

	// returns a rect pointing to the sprites current frame 
	RECT getRect();

	// returns current facing of sprite
	DIRS getFacing();
	// sets the facing of sprite
	void setFacing(DIRS newFacing , bool changeMoveXY);

	// sets new facing and movexy at same time, using int
	void setNewDIR(int newDIR);

	bool isMoving();				// returns true if sprite is moving
	void setMoving(bool newState);	// sets if sprite is moving

	void setRhombus();		// creates / sets the sprites rectangle/rhombus
	RECT getRhombus();		// this will return the "iso diamond" at the bottom of the sprite
	
	POINT getScreenPT();	// returns current screen xy of sprite as POINT

	// defines the screen walkable/collision border for this sprite
	void setBorder(int left, int right, int top, int bottom);

	// returns TRUE if sprite is within its defined border, FALSE otherwise
	bool hitBorder();

	// sets the sprites current STATE 
	void setSTATE(STATEtype newS);

	// returns the curren STATE of sprite
	int getSTATE();
//	INACTIVE	= 0,
//	ACTIVE		= 1,		// moving
//	CASTING		= 2,
//	ATTACKING	= 3,
//	DYING		= 6
//	STATEtype STATE;

	// returns the anchor point, which is in center of the sprite collision rectangle
	POINT getAnchor();

	// constructor for new sprites ... curFrame will always be (1) .. scale and rotation not currently implemented
	// and this is for non-animated sprites
	CSprite(int X, int Y, int Width, int Height, char *Filename);

	// destructor
	~CSprite(void); 

private:
	STATEtype STATE;

	RECT border;
	RECT rhombus;

	DIRS facingDir;       // direction sprite is facing, used for choosing correct sprite from sprite sheet
	DIRS lastDir;

	bool moving;

    int x,y;
	int width,height;
	int mana; // current mana

    int movex,movey;
    int curframe,lastframe;
    int animdelay,animcount;
	int columns;         // used to grabs sprites from sprite map
	int frames_per_anim; // using sprite sheet, will need this

	LPDIRECT3DTEXTURE9 _image;

	void LoadImage(char *Filename);

}; // end of CSprite class

#endif
