#include <ace/managers/system.h>
#include <ace/managers/viewport/simplebuffer.h>
#include "defines.h"
#include "structures.h"
#include <ace/managers/blit.h>

extern struct wicher wicher;

void blitWicherAnim(UBYTE dir, tBitMap *pBack, tBitMap *tileset)
{
	switch (dir) // setting up 'pixel position' for blitting fly animation
	{				   // based on position on level map array, every frame
	case RIGHT:
		wicher.blitPosX = (wicher.mapPosX * TS) + (wicher.animCount* WICHER_ANIM_SPEED);
		break;
	case LEFT:
		wicher.blitPosX = (wicher.mapPosX * TS) - (wicher.animCount * WICHER_ANIM_SPEED);
		break;
	case UP:
		wicher.blitPosY = (wicher.mapPosY * TS) - (wicher.animCount * WICHER_ANIM_SPEED);
		break;
	case DOWN:
		wicher.blitPosY = (wicher.mapPosY * TS) + (wicher.animCount* WICHER_ANIM_SPEED);
		break;
	default:
		break;
	}

	if (wicher.animTick == 0 || wicher.animTick == 1)
	{
		if (wicher.animCount == 0)
		{	// this might be unnecessary
			// blitRect(s_pVpManagerMenu->pBack,blitPosX,blitPosY,TS,TS,0);
			// blitCopy(s_pTileset,0,wicherFace,s_pVpManagerMenu->pBack,blitPosX,blitPosY,TS,TS,MINTERM_COOKIE);
		}
		else
		{
			switch (dir) // this is for proper background bliting each frame
			{				   // blits bg on the oposite side than direction
			case RIGHT:
				blitRect(pBack, wicher.blitPosX - WICHER_ANIM_SPEED, wicher.blitPosY, TS, TS, 0);
				break;
			case LEFT:
				blitRect(pBack, wicher.blitPosX + WICHER_ANIM_SPEED, wicher.blitPosY, TS, TS, 0);
				break;
			case UP:
				blitRect(pBack, wicher.blitPosX, wicher.blitPosY + WICHER_ANIM_SPEED, TS, TS, 0);
				break;
			case DOWN:
				blitRect(pBack, wicher.blitPosX, wicher.blitPosY - WICHER_ANIM_SPEED, TS, TS, 0);
				break;

			default:
				break;
			}
			blitCopy(tileset, 0, wicher.face, pBack, wicher.blitPosX, wicher.blitPosY, TS, TS, MINTERM_COOKIE);
		}
	}
}