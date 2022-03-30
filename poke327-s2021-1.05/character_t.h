#ifndef CHARACTER_T_H
#define CHARACTER_T_H

#include "poke327.h"
#include "character.h"

class character_t
{
private:
public:
    character_t();
    virtual ~character_t();
    pair_t pos;
    char symbol;
    int next_turn;
};

class pc_t : public character_t
{
private:
public:
    pc_t();
    virtual ~pc_t();

}

class npc_t : public character_t
{
private:
public:
    npc_t();
    virtual ~npc_t();
    character_type_t ctype;
    movement_type_t mtype;
    int defeated;
    pair_t dir;

};

#endif