#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "heap.h"
#include "pokemon.h"
#include "moves.h"


void parse_pokemon(pokemon p [1092], std::ifstream  &i){
    std::string first;
    std::getline(i, first);
    
    int j;
    for (j=0; j<1092; j++){
        
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

        p[j].set_values(stoi(id), name, stoi(sid), stoi(h), stoi(w), stoi(be), stoi(o), stoi(isd));
    }

}


void parse_moves(moves p [844], std::ifstream  &i){
    std::string first;
    std::getline(i, first);
    
    int j;
    for (j=0; j<844; j++){
        std::string id;
        //std::cout << i.peek() << " " << j << std::endl;
        if(i.peek() == 44){
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

        p[j].set_values(stoi(id), identifier, stoi(gen_id), stoi(type_id), stoi(power), stoi(pp), stoi(accuracy), stoi(priority), stoi(target_id), stoi(damage_class_id), stoi(effect_id), stoi(effect_chance), stoi(contest_type_id), stoi(contest_effect_id), stoi(super_contest_effect_id));
        //p[0].set_values(1, "test", 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
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
        }
    }

    pokemon poke [1092];
    moves move [844];

    if (fname == "pokemon"){
        parse_pokemon(poke, i);
        int j; 
        for(j=0; j<1092; j++){
            poke[j].print();
        }
    }

    if (fname == "moves"){
        parse_moves(move, i);
        int j; 
        for(j=0; j<844; j++){
            move[j].print();
        }
    }
    return 0;
}
