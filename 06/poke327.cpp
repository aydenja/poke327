#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "heap.h"
#include "pokemon.h"
#include "moves.h"
#include "pokemon_moves.h"
#include "type_names.h"
#include "pokemon_species.h"
#include "experience.h"


void parse_pokemon(pokemon *p [1092], std::ifstream  &i){
    std::string first;
    std::getline(i, first);
    
    int j;
    for (j=0; j<1092; j++){
        p[j] = new pokemon();
        std::string id;
        std::getline(i, id, ',');

        std::string name;
        std::getline(i, name, ',');

        std::string sid;
        std::getline(i, sid, ',');

        std::string h;
        std::getline(i, h, ',');

        std::string w;
        std::getline(i, w, ',');

        std::string be;
        std::getline(i, be, ',');

        std::string o;
        std::getline(i, o, ',');

        std::string isd;
        std::getline(i, isd);

        p[j]->set_values(stoi(id), name, stoi(sid), stoi(h), stoi(w), stoi(be), stoi(o), stoi(isd));
    }

}


void parse_moves(moves *p [844], std::ifstream  &i){
    std::string first;
    std::getline(i, first);
    
    int j;
    for (j=0; j<844; j++){
        p[j] = new moves();
        
        std::string id;
        //std::cout << i.peek() << " " << j << std::endl;
        if(i.peek() == ','){
            i.get();
            id = "-1";
        }
        else {
            std::getline(i, id, ',');
        }


        std::string identifier;
        if(i.peek() == ','){
            i.get();
            identifier = "-1";
        }
        else {
            std::getline(i, identifier, ',');
        }

        std::string gen_id;
        if(i.peek() == ','){
            i.get();
            gen_id = "-1";
        }
        else {
            std::getline(i, gen_id, ',');
        }

        std::string type_id;
        if(i.peek() == ','){
            i.get();
            type_id = "-1";
        }
        else {
            std::getline(i, type_id, ',');
        }

        std::string power;
        if(i.peek() == ','){
            i.get();
            power = "-1";
        }
        else {
            std::getline(i, power, ',');
        }

        std::string pp;
        if(i.peek() == ','){
            i.get();
            pp = "-1";
        }
        else {
            std::getline(i, pp, ',');
        }

        std::string accuracy;
        if(i.peek() == ','){
            i.get();
            accuracy = "-1";
        }
        else {
            std::getline(i, accuracy, ',');
        }

        std::string priority;
        if(i.peek() == ','){
            i.get();
            priority = "-1";
        }
        else {
            std::getline(i, priority, ',');
        }

        std::string target_id;
        if(i.peek() == ','){
            i.get();
            target_id = "-1";
        }
        else {
            std::getline(i, target_id, ',');
        }

        std::string damage_class_id;
        if(i.peek() == ','){
            i.get();
            damage_class_id = "-1";
        }
        else {
            std::getline(i, damage_class_id, ',');
        }

        std::string effect_id;
        if(i.peek() == ','){
            i.get();
            effect_id = "-1";
        }
        else {
            std::getline(i, effect_id, ',');
        }

        std::string effect_chance;
        if(i.peek() == ','){
            i.get();
            effect_chance = "-1";
        }
        else {
            std::getline(i, effect_chance, ',');
        }

        std::string contest_type_id;
        if(i.peek() == ','){
            i.get();
            contest_type_id = "-1";
        }
        else {
            std::getline(i, contest_type_id, ',');
        }

        std::string contest_effect_id;
        if(i.peek() == ','){
            i.get();
            contest_effect_id = "-1";
        }
        else {
            std::getline(i, contest_effect_id, ',');
        }

        std::string super_contest_effect_id;
        if(i.peek() == '\n'){
            i.get();
            super_contest_effect_id = "-1";
        }
        else {
            std::getline(i, super_contest_effect_id);
        }

        p[j]->set_values(stoi(id), identifier, stoi(gen_id), stoi(type_id), stoi(power), stoi(pp), stoi(accuracy), stoi(priority), stoi(target_id), stoi(damage_class_id), stoi(effect_id), stoi(effect_chance), stoi(contest_type_id), stoi(contest_effect_id), stoi(super_contest_effect_id));
        //p[0].set_values(1, "test", 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
    }

}

void parse_pokemon_moves(pokemon_moves *p [528238], std::ifstream  &i){
    std::string first;
    std::getline(i, first);
    
    int j;
    for (j=0; j<528238; j++){
        p[j] = new pokemon_moves();
        std::string id;
        if(i.peek() == ','){
            i.get();
            id = "-1";
        }
        else {
            std::getline(i, id, ',');
        }

        std::string version_group_id;
        if(i.peek() == ','){
            i.get();
            version_group_id = "-1";
        }
        else {
            std::getline(i, version_group_id, ',');
        }

        //std::cout << version_group_id << std::endl;

        std::string move_id;
        if(i.peek() == ','){
            i.get();
            move_id = "-1";
        }
        else {
            std::getline(i, move_id, ',');
        }

        std::string pokemon_move_method_id;
        if(i.peek() == ','){
            i.get();
            pokemon_move_method_id = "-1";
        }
        else {
            std::getline(i, pokemon_move_method_id, ',');
        }

        std::string level;
        if(i.peek() == ','){
            i.get();
            level = "-1";
        }
        else {
            std::getline(i, level, ',');
        }

        std::string order;
        if(i.peek() == '\n'){
            i.get();
            order = "-1";
        }
        else {
            std::getline(i, order);
        }
        
        //std::cout << j << id << version_group_id<<  move_id<<  pokemon_move_method_id << level  << order  <<  std::endl;
        p[j]->set_values(stoi(id), stoi(version_group_id), stoi(move_id), stoi(pokemon_move_method_id), stoi(level), stoi(order));
        
    }

}

void parse_type_names(type_names *p [40], std::ifstream  &i){
    
    std::string first;
    std::getline(i, first);
    
    int j;
    while (j<40)
    {
        int len = i.tellg();
        std::string line;
        std::string tmp;
        //getline(i, line);
        std::getline(i, tmp, ',');
        std::getline(i, tmp, ',');

        if(stoi(tmp) == 1 || stoi(tmp) == 9 ){
            p[j] = new type_names();
            i.seekg(len ,std::ios_base::beg);
            std::string id;
            std::getline(i, id, ',');

            std::string lid;
            std::getline(i, lid, ',');

            std::string name;
            std::getline(i, name);

            p[j]->set_values(stoi(id), stoi(lid), name);
            j++;
        }
        else{
             std::getline(i, tmp);
        }

    }
    
}

std::string getNext(std::ifstream  &i){
    std::string id;
    if(i.peek() == ','){
        i.get();
        id = "-1";
    }
    else {
        std::getline(i, id, ',');
    }
    return id;
}

void parse_pokemon_species(pokemon_species *p [898], std::ifstream  &i){
    
    std::string first;
    std::getline(i, first);

    int j;
    for (j=0; j<898; j++){
        
        p[j] = new pokemon_species();
        std::string id = getNext(i);
        std::string identifier = getNext(i);
        std::string generation_id = getNext(i);
        std::string evolves_from_species_id = getNext(i);
        std::string evolution_chain_id = getNext(i);
        std::string color_id = getNext(i);
        std::string shape_id = getNext(i);
        std::string habitat_id = getNext(i);
        std::string gender_rate = getNext(i);
        std::string capture_rate = getNext(i);
        std::string base_happiness = getNext(i);
        std::string is_baby = getNext(i);
        std::string hatch_counter = getNext(i);
        std::string has_gender_differences = getNext(i);
        std::string growth_rate_id = getNext(i);
        std::string forms_switchable = getNext(i);
        std::string is_legendary = getNext(i);
        std::string is_mythical = getNext(i);
        std::string order = getNext(i);
        std::string conquest_order;
        if(i.peek() == '\n'){
            i.get();
            conquest_order = "-1";
        }
        else {
            std::getline(i, conquest_order);
        }

        p[j]->set_values(stoi(id), identifier, stoi(generation_id), stoi(evolves_from_species_id), stoi(evolution_chain_id), stoi(color_id), stoi(shape_id), stoi(habitat_id), stoi(gender_rate), stoi(capture_rate), stoi(base_happiness), stoi(is_baby), stoi(hatch_counter), stoi(has_gender_differences), stoi(growth_rate_id), stoi(forms_switchable), stoi(is_legendary), stoi(is_mythical), stoi(order), stoi(conquest_order));
    }

}

void parse_experience(experience *p [600], std::ifstream  &i){
    
    std::string first;
    std::getline(i, first);
    
    int j;
    for (j=0; j<600; j++){
        p[j] = new experience();
        std::string growth_rate_id;
        std::getline(i, growth_rate_id, ',');

        std::string level;
        std::getline(i, level, ',');

        std::string experience;
        std::getline(i, experience);

        p[j]->set_values(stoi(growth_rate_id), stoi(level), stoi(experience));
    }
    
}

int main(int argc, char const *argv[])
{
    std::string fname;
    if (argc >= 2){
        fname =argv[1];
        //std::cout << fname << std::endl; 
    }

    std::string try1("/share/cs327/pokedex/pokedex/data/csv/" + fname + ".csv");
    std::string home(getenv("HOME"));
    std::string try2( home + "/.poke327" + fname + ".csv");
    std::string try3(fname + ".csv");

    std::ifstream i(try1);

    if(i.fail()){
        i.open(try2);
        if(i.fail()){
            i.open(try3);
            //std::cout << i.fail() << std::endl;
            if(i.fail()){
                std::cout << "Could not find file" << std::endl;
            }
        }
    }

    pokemon *poke [1092];
    moves *move [844];
    pokemon_moves *p_moves[528238];
    type_names *t_names[40];
    pokemon_species *p_species[898];
    experience *ex[600];
    

    if (fname == "pokemon"){
        parse_pokemon(poke, i);
        int j; 
        for(j=0; j<1092; j++){
            poke[j]->print();
        }
    }

    if (fname == "moves"){
        parse_moves(move, i);
        int j; 
        for(j=0; j<844; j++){
            move[j]->print();
        }
    }

    if (fname == "pokemon_moves"){
        parse_pokemon_moves(p_moves, i);
        int j; 
        for(j=0; j<528238; j++){
            p_moves[j]->print();
        }
    }

    if (fname == "type_names"){
        parse_type_names(t_names, i);
        int j; 
        for(j=0; j<40; j++){
            t_names[j]->print();
        }
    }

    if (fname == "pokemon_species"){
        parse_pokemon_species(p_species, i);
        int j; 
        for(j=0; j<898; j++){
            p_species[j]->print();
        }
    }

    if (fname == "experience"){
        parse_experience(ex, i);
        int j; 
        for(j=0; j<600; j++){
            ex[j]->print();
        }
    }


    return 0;
}