// Collision routines
// Jeff Yaskus
// framework source code file

#include <d3d9.h>
#include <d3dx9.h>

#include "collision.h"

bool isBetween(int value, int min, int max)
{
	return (value >= min && value <= max);
}

int Collision(CSprite *sprite1, CSprite *sprite2)
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

DIRS Orientation(CSprite *sprite1, CSprite *defendent)
{
	// FROM:
	RECT r;
	r.left   = sprite1->getX() + 1;
	r.top    = sprite1->getY() + 1;
	r.right  = sprite1->getX() + sprite1->getWidth()  -1;
	r.bottom = sprite1->getY() + sprite1->getHeight() -1;

	// TO:
	int centerx = defendent->getX() + defendent->getWidth() /2;
	int centery = defendent->getY() + defendent->getHeight() /2;

	// above
	if (centery < r.top)
	{
		if (centerx < r.left)
			return NW;
		else if (centerx > r.right)
			return NE;
		else
			return NORTH;
	}

	// below
	if (centery > r.bottom)
	{
		if (centerx < r.left)
			return SW;
		else if (centerx > r.right)
			return SE;
		else
			return SOUTH;
	}

	// same Y location
	if (centerx < r.left)
		return WEST;
	if (centerx > r.right)
		return EAST;

	//default
	return EAST;
};