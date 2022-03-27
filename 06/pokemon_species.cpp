#include "pokemon_species.h"
#include <iostream>
#include <string>


pokemon_species::pokemon_species(){
    
}

pokemon_species::pokemon_species(int id, std::string identifier, int generation_id, int evolves_from_species_id, int evolution_chain_id, int color_id, int shape_id, int habitat_id, int gender_rate, int capture_rate, int base_happiness, int is_baby, int hatch_counter, int has_gender_differences, int growth_rate_id, int forms_switchable, int is_legendary, int is_mythical, int order, int conquest_order){
    this->id= id;
    this->identifier =  identifier;
    this->generation_id =  generation_id;
    this->evolves_from_species_id = evolves_from_species_id;
    this->evolution_chain_id =  evolution_chain_id;
    this->color_id =  color_id;
    this->shape_id =  shape_id;
    this->habitat_id =  habitat_id;
    this->gender_rate =  gender_rate;
    this->capture_rate =  capture_rate;
    this->base_happiness =  base_happiness;
    this->is_baby =  is_baby;
    this->hatch_counter =  hatch_counter;
    this->has_gender_differences =  has_gender_differences;
    this->growth_rate_id =  growth_rate_id;
    this->forms_switchable =  forms_switchable;
    this->is_legendary =  is_legendary;
    this->is_mythical =  is_mythical;
    this->order =  order;
    this->conquest_order =  conquest_order;
}

void pokemon_species::set_values(int id, std::string identifier, int generation_id, int evolves_from_species_id, int evolution_chain_id, int color_id, int shape_id, int habitat_id, int gender_rate, int capture_rate, int base_happiness, int is_baby, int hatch_counter, int has_gender_differences, int growth_rate_id, int forms_switchable, int is_legendary, int is_mythical, int order, int conquest_order){
    this->id= id;
    this->identifier =  identifier;
    this->generation_id =  generation_id;
    this->evolves_from_species_id = evolves_from_species_id;
    this->evolution_chain_id =  evolution_chain_id;
    this->color_id =  color_id;
    this->shape_id =  shape_id;
    this->habitat_id =  habitat_id;
    this->gender_rate =  gender_rate;
    this->capture_rate =  capture_rate;
    this->base_happiness =  base_happiness;
    this->is_baby =  is_baby;
    this->hatch_counter =  hatch_counter;
    this->has_gender_differences =  has_gender_differences;
    this->growth_rate_id =  growth_rate_id;
    this->forms_switchable =  forms_switchable;
    this->is_legendary =  is_legendary;
    this->is_mythical =  is_mythical;
    this->order =  order;
    this->conquest_order =  conquest_order;
}

pokemon_species::~pokemon_species(){
}

void pokemon_species::print(){
    //std::cout << "ID: " << type_id << " | ";
    std::cout << "id: " <<id << " | ";
    std::cout << "identifier: " <<identifier << " | ";
    std::cout << "generation_id: " <<generation_id << " | ";
    std::cout << "evolves_from_species_id: " <<evolves_from_species_id << " | ";
    std::cout << "evolution_chain_id: " <<evolution_chain_id << " | ";
    std::cout << "color_id: " <<color_id << " | ";
    std::cout << "shape_id: " <<shape_id << " | ";
    std::cout << "habitat_id: " <<habitat_id << " | ";
    std::cout << "gender_rate: " <<gender_rate << " | ";
    std::cout << "capture_rate: " <<capture_rate << " | ";
    std::cout << "base_happiness: " <<base_happiness << " | ";
    std::cout << "is_baby: " <<is_baby << " | ";
    std::cout << "hatch_counter: " <<hatch_counter << " | ";
    std::cout << "has_gender_differences: " <<has_gender_differences << " | ";
    std::cout << "growth_rate_id: " <<growth_rate_id << " | ";
    std::cout << "forms_switchable: " <<forms_switchable << " | ";
    std::cout << "is_legendary: " <<is_legendary << " | ";
    std::cout << "is_mythical: " <<is_mythical << " | ";
    std::cout << "order: " <<order << " | ";
    std::cout << "conquest_order: " <<conquest_order << std::endl;
}
