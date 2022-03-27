#ifndef EXPERIENCE_H
#define EXPERIENCE_H
#include <iostream>
#include <string>

class experience
{
private:
    int growth_rate_id;
    int level;
    int experience_v;
    
public:
    experience();
    experience(int growth_rate_id, int level, int experience_v);
    ~experience();
    void set_values(int growth_rate_id, int level, int experience_v);

    void print();
};

#endif