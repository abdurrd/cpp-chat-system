#include "Client.hpp"

class User {
private:
        std::string _name;

public:
        User(std::string name);

        std::string get_name();

        User login(Client &client);
        bool signup();
};
