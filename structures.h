#ifndef STRUCTURES_H
#define STRUCTURES_H

struct wicher {
    UBYTE mapPosX;
    UBYTE mapPosY;
    int blitPosX;
    int blitPosY;
    UBYTE state;
    UBYTE face;
    UBYTE animTick;
    UBYTE animCount;  
};

#endif