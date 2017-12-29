//
// Created by sobieg on 19.12.17.
//

#include "meter.h"

bool comp(struct value val1, struct value val2) {
    return (mktime(&val1.date) < mktime(&val2.date));
}

meter::meter(std::string name) {
    name = name;
}

void meter::add_value(struct tm date, unsigned int value) {
    struct value val;
    val.date = date;
    val.value = value;
    values.push_back(val);
    std::sort(values.begin(), values.end(), comp);
}

value meter::get_last() {
    return values.back();
}

value meter::get_first() {
    return values.front();
}

std::vector<value> meter::get_history() {
    return values;
}

std::string meter::get_name() {
    return name;
}

void meter::set_user_id(unsigned long uid) {
    user_id = uid;
}


