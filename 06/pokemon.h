#ifndef POKEMON_H
#define POKEMON_H
#include <iostream>
#include <string>

class pokemon
{
private:
    int id;
    std::string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;
    
public:
    pokemon();
    pokemon(int id, std::string identifier, int sid, int h, int w, int be, int o, int isd);
    ~pokemon();
    void set_values(int id, std::string identifier, int sid, int h, int w, int be, int o, int isd);

    void print();
};

#endif
