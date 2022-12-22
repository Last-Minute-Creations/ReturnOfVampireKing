#ifndef STRUCTURES_H
#define STRUCTURES_H

struct wicher {
    // movement handling
    UBYTE mapPosX;
    UBYTE mapPosY;
    int blitPosX;
    int blitPosY;
    UBYTE state;
    UBYTE face;
    UBYTE animTick;
    UBYTE animCount;  
    
};

struct stats {
    UBYTE power;
    UBYTE endurance;
    UBYTE energy;
};

#endif