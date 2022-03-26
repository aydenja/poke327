#ifndef MOVES_H
#define MOVES_H
#include <iostream>
#include <string>

class moves
{
private:
    int id;
    std::string identifier;
    int gen_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;
    
public:
    moves();
    moves(int id, std::string identifier, int gen_id, int type_id,int power,int pp,int accuracy,int priority,int target_id,int damage_class_id,int effect_id,int effect_chance,int contest_type_id,int contest_effect_id,int super_contest_effect_id);
    ~moves();
    void set_values(int id, std::string identifier, int gen_id, int type_id,int power,int pp,int accuracy,int priority,int target_id,int damage_class_id,int effect_id,int effect_chance,int contest_type_id,int contest_effect_id,int super_contest_effect_id);

    void print();
};

#endif
