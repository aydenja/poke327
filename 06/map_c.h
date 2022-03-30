#ifndef MAP_C_H
#define MAP_C_H
#include "poke327.h"


class map_c
{
public:
    map_c();
    virtual ~map_c();
    terrain_type_t map[MAP_Y][MAP_X];
    uint8_t height[MAP_Y][MAP_X];
    character_t *cmap[MAP_Y][MAP_X];
    heap_t turn;
    int32_t num_trainers;
    int8_t n, s, e, w;
};


#endif