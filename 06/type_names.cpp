#include "type_names.h"
#include <iostream>
#include <string>


type_names::type_names(){
    
}

type_names::type_names(int type_id, int local_language_id, std::string name){
    this->type_id = type_id;
    this->local_language_id= local_language_id;
    this->name= name;
}

void type_names::set_values(int type_id, int local_language_id, std::string name){
    this->type_id = type_id;
    this->local_language_id= local_language_id;
    this->name= name;
}

type_names::~type_names(){
}

void type_names::print(){
    std::cout << "ID: " << type_id << " | ";
    std::cout << "LID: " << local_language_id << " | ";
    std::cout << "Name: " << name << std::endl;
}
