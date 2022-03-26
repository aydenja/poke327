#include "moves.h"
#include <iostream>
#include <string>


moves::moves(){
     id = -1;
    identifier = "-1";
     gen_id = -1;
     type_id = -1;
     power = -1;
     pp = -1;
     accuracy = -1;
     priority = -1;
     target_id = -1;
     damage_class_id = -1;
     effect_id = -1;
     effect_chance = -1;
     contest_type_id = -1;
     contest_effect_id = -1;
     super_contest_effect_id = -1;
}

moves::moves(int id, std::string identifier, int gen_id, int type_id, int power, int pp,int accuracy, int priority, int target_id, int damage_class_id,int effect_id,int effect_chance,int contest_type_id,int contest_effect_id,int super_contest_effect_id){
    this->id = id;
    this->identifier = identifier;
    this->gen_id = gen_id;
    this->type_id = type_id;
    this->power = power;
    this->pp = pp;
    this->accuracy = accuracy;
    this->priority = priority;
    this->target_id = target_id;
    this->damage_class_id = damage_class_id;
    this->effect_id = effect_id;
    this->effect_chance = effect_chance;
    this->contest_type_id = contest_type_id;
    this->contest_effect_id = contest_effect_id;
    this->super_contest_effect_id = super_contest_effect_id;
}

void moves::set_values(int id, std::string identifier, int gen_id, int type_id,int power,int pp,int accuracy,int priority,int target_id,int damage_class_id,int effect_id,int effect_chance,int contest_type_id,int contest_effect_id,int super_contest_effect_id){
    this->id = id;
    this->identifier = identifier;
    this->gen_id = gen_id;
    this->type_id = type_id;
    this->power = power;
    this->pp = pp;
    this->accuracy = accuracy;
    this->priority = priority;
    this->target_id = target_id;
    this->damage_class_id = damage_class_id;
    this->effect_id = effect_id;
    this->effect_chance = effect_chance;
    this->contest_type_id = contest_type_id;
    this->contest_effect_id = contest_effect_id;
    this->super_contest_effect_id = super_contest_effect_id;
}

moves::~moves(){
}

void moves::print(){
    if(id != -1){
        std::cout << "ID: " << id << " | ";
    }
    if(identifier != "-1"){
       std::cout << "Identifier: " << identifier << " | ";
    }
    if(gen_id != -1){
        std::cout << "Gen ID: " << gen_id << " | ";
    }
    if(type_id != -1){
        std::cout << "type_id: " << type_id << " | ";
    }
    if(power != -1){
        std::cout << "power: " << power << " | ";
    }
    if(pp != -1){
        std::cout << "pp: " << pp << " | ";
    }
    if(accuracy != -1){
        std::cout << "accuracy: " << accuracy << " | ";
    }
    if(priority != -1){
        std::cout << "priority: " << priority << " | ";
    }
    if(target_id != -1){
        std::cout << "target_id: " << target_id << " | ";
    }
    if(damage_class_id != -1){
        std::cout << "damage_class_id: " << damage_class_id << " | ";
    }
    if(effect_id != -1){
        std::cout << "effect_id: " << effect_id << " | ";
    }
    if(effect_chance != -1){
        std::cout << "effect_chance: " << effect_chance << " | ";
    }
    if(contest_type_id != -1){
        std::cout << "contest_type_id: " << contest_type_id << " | ";
    }
    if(contest_effect_id != -1){
        std::cout << "contest_effect_id: " << contest_effect_id << " | ";
    }
    if(super_contest_effect_id != -1){
        std::cout << "super_contest_effect_id: " << super_contest_effect_id;
    }
    std::cout << std::endl;

}