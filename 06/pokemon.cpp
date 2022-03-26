#include "pokemon.h"
#include <iostream>
#include <string>


pokemon::pokemon(){
    id = -1;
    identifier = "-1";
    species_id = -1;
    height = -1;
    weight =-1;
    base_experience = -1; 
    order = -1;
    is_default = -1;
}

pokemon::pokemon(int id, std::string identifier, int sid, int h, int w, int be, int o, int isd){
    this->id = id;
    this->identifier = identifier;
    species_id = sid;
    height = h;
    weight =w;
    base_experience = be; 
    order = o;
    is_default = isd;
}

void pokemon::set_values(int id, std::string identifier, int sid, int h, int w, int be, int o, int isd){
    this->id = id;
    this->identifier = identifier;
    species_id = sid;
    height = h;
    weight =w;
    base_experience = be; 
    order = o;
    is_default = isd;
}

pokemon::~pokemon(){
}

void pokemon::print(){
    std::cout << "ID: " << id << " | ";
    std::cout << "Identifier: " << identifier << " | ";
    std::cout << "Species id: " << species_id << " | ";
    std::cout << "Height: " << height << " | ";
    std::cout << "Weight: " << weight << " | ";
    std::cout << "Base Experience: " << base_experience << " | ";
    std::cout << "Order: " << order << " | ";
    std::cout << "Is default: " << is_default << std::endl;
}