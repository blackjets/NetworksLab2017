//
// Created by sobieg on 19.12.17.
//

#ifndef ZHKH_METER_H
#define ZHKH_METER_H

#include <string>
#include <vector>
#include <algorithm>


struct value {
    struct tm date;
    unsigned int value;
};

class meter {
private:
    std::string name;
    std::vector<value> values;
    unsigned long user_id;

public:
    meter(std::string name);
    value get_last();
    value get_first();
    std::vector<value> get_history();
    void add_value(std::string date, unsigned int value);
    std::string get_name();
    void set_user_id(unsigned long uid);
};


#endif //ZHKH_METER_H
