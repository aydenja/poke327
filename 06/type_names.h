#ifndef TYPE_NAMES_H
#define TYPE_NAMES_H
#include <iostream>
#include <string>

class type_names
{
private:
    int type_id;
    int local_language_id;
    std::string name;
    
public:
    type_names();
    type_names(int type_id, int local_language_id, std::string name);
    ~type_names();
    void set_values(int type_id, int local_language_id, std::string name);

    void print();
};

#endif