#include <ace/managers/system.h>
#include "defines.h"

void drawMap()
{ // todo - pass map name as argument (pointers problems etc)
	for (UBYTE i = 0; i < MAP_WIDTH; ++i)
	{
		for (UBYTE j = 0; j < MAP_HEIGHT; ++j)
		{
			if (mapCurrent[j][i] == EMPTY_TILE)
			{
				blitRect(s_pVpManagerMenu->pBack, i * TS, j * TS, TS, TS, 0);
			}
			if (mapCurrent[j][i] == METEORITE)
			{
				blitCopy(s_pTileset, 0, 0, s_pVpManagerMenu->pBack, i * TS, j * TS, TS, TS, MINTERM_COOKIE);
			}
			if (mapCurrent[j][i] == WICHER)
			{
				wicher.mapPosX = i;
				wicher.mapPosY = j;
				wicher.blitPosX = i * TS;
				wicher.blitPosY = j * TS;
				blitCopy(s_pTileset, 0, wicher.face, s_pVpManagerMenu->pBack, i * TS, j * TS, TS, TS, MINTERM_COOKIE);
			}
		}
	}
}