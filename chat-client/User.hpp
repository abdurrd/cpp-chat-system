#pragma once
#include <iostream>

class User {
private:
        std::string             _name;
        std::string             _password;
        int                     _size;
        std::array<int, 10>     _groups{};

public:
        User(std::string name, std::string password);

        void grp_push(int grp_id){
                _groups[_size] = grp_id;
                ++_size;
        }

        std::string get_name();
        std::string get_pass();
};
