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

struct stats {
    int att;
    UBYTE spear_level;
    int def;
    UBYTE shield_level;
    int hp;
    int batteryCapacity;
};

struct collected {
    int coal;
    int capacitors;
    int resistors;
    int energy;
};

struct sordan_prices {
    int capacitor;
    int resistor;
    int scrap;
};

struct rastport_prices {
    int capacitor;
    int resistor;
    int coal;
};

#endif