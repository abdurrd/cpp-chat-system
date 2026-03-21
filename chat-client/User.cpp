#include "User.hpp"

User::User(std::string name) : _name(name) {}

std::string User::get_name() {
        return _name;
}

User User::login(Client &client){
        std::string name;
        std::cout << "Username: ";
        std::cin >> name;
        std::cout << "\n";

        std::string password;
        std::cout << "Password: ";
        std::cin >> password;
        std::cout << "\n";

        bool auth = client.login(name, password);
        if(!auth) ERR("Login in failed");

        User user(name);

        return user;
}

