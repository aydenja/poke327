#include "pokemon_moves.h"
#include <iostream>
#include <string>


pokemon_moves::pokemon_moves(){
    pokemon_id = -1;
    version_group_id = -1;
    move_id = -1;
    pokemon_move_method_id = -1;
    level = -1;
    order = -1;
}

pokemon_moves::pokemon_moves(int pokemon_id, int version_group_id, int move_id, int pokemon_move_method_id, int level, int order){
    this->pokemon_id = pokemon_id;
    this->version_group_id = version_group_id;
    this->move_id = move_id;
    this->pokemon_move_method_id = pokemon_move_method_id;
    this->level = level;
    this->order = order;
}

void pokemon_moves::set_values(int pokemon_id, int version_group_id, int move_id, int pokemon_move_method_id, int level, int order){
    this->pokemon_id = pokemon_id;
    this->version_group_id = version_group_id;
    this->move_id = move_id;
    this->pokemon_move_method_id = pokemon_move_method_id;
    this->level = level;
    this->order = order;
}

pokemon_moves::~pokemon_moves(){
}

void pokemon_moves::print(){
    if(pokemon_id != -1){
        std::cout << "ID: " << pokemon_id << " | ";
    }
    if(version_group_id != -1){
        std::cout << "Version GID: " << version_group_id << " | ";
    }
    if(move_id != -1){
        std::cout << "Move ID: " << move_id << " | ";
    }
    if(pokemon_move_method_id != -1){
        std::cout << "P_Move_Method ID: " << pokemon_move_method_id << " | ";
    }
    if(level != -1){
        std::cout << "Level: " << level << " | ";
    }
    if(order != -1){
        std::cout << "Order: " << order;
    }
    
    std::cout << std::endl;

}