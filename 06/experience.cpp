#include "experience.h"
#include <iostream>
#include <string>


experience::experience(){
    
}

experience::experience(int growth_rate_id, int level, int experience_v){
    this->growth_rate_id = growth_rate_id;
    this->level= level;
    this->experience_v= experience_v;
}

void experience::set_values(int growth_rate_id, int level, int experience_v){
    this->growth_rate_id = growth_rate_id;
    this->level= level;
    this->experience_v= experience_v;
}

experience::~experience(){
}

void experience::print(){
    std::cout << "Growth Rate ID: " << growth_rate_id << " | ";
    std::cout << "Level: " << level << " | ";
    std::cout << "Name: " << experience_v << std::endl;
}
