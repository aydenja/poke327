#ifndef CHARACTER_C_H
#define CHARACTER_C_H

#include "poke327.h"
#include "character.h"

class character_c
{
private:
public:
    character_c();
    virtual ~character_c();
    pair_t pos;
    char symbol;
    int next_turn;
};

class pc_c : public character_c
{
private:
public:
    pc_c();
    virtual ~pc_c();

};

class npc_c : public character_c
{
private:
public:
    npc_c();
    virtual ~npc_c();
    character_type_t ctype;
    movement_type_t mtype;
    int defeated;
    pair_t dir;

};

#endif