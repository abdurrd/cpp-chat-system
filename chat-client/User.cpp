#include "User.hpp"

User::User(std::string name, std::string password) : _name(name), _password(password) {}

std::string User::get_name() {
        return _name;
}

std::string User::get_pass() {
        return _password;
}

