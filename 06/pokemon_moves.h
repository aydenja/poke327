#ifndef POKEMON_MOVES_H
#define POKEMON_MOVES_H
#include <iostream>
#include <string>

class pokemon_moves
{
private:
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;
    
public:
    pokemon_moves();
    pokemon_moves(int pokemon_id, int version_group_id, int move_id, int pokemon_move_method_id, int level, int order);
    ~pokemon_moves();
    void set_values(int pokemon_id, int version_group_id, int move_id, int pokemon_move_method_id, int level, int order);

    void print();
};

#endif