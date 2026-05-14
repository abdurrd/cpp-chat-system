#include "TransportLayer.hpp"

std::string TransportLayer::proto_to_s(Protocol proto) {
        return std::to_string(static_cast<int>(proto));
}

void TransportLayer::send_payload() {
        if(_payload.length() == 0) return;

        send(_client_fd, _payload.c_str(), _payload.length(), 0);
        _payload.clear();
}

std::string TransportLayer::await_payload() {
        char buffer[1024];
        int read = recv(_client_fd, buffer, 1024, 0);
        if(read <= 0) {
                std::cout << " - Connection interupted\n";
                return "";
        }

        buffer[read] = '\0';
        return std::string(buffer);
}

int TransportLayer::resolve_payload(std::string serv_payload) {
        std::istringstream stream(serv_payload);

        Protocol proto;
        stream >> proto; 
        std::string data = stream.str().substr(stream.tellg());
        data = data.substr(data.find_first_not_of(" \t\n"));

        switch(proto) {
                case Protocol::REGISTER:
                {
                        *_loggedin = true;
                        return 0;
                }
                case Protocol::LOGIN:
                {
                        *_loggedin = true;
                        if(data[0] == 0) return 1;

                        auto groups = data 
                                | std::views::split(GROUP_SEP) 
                                | std::views::drop(1);

                        for(auto group: groups) {
                                auto segs = group 
                                        | std::views::split(FIELD_SEP) 
                                        | std::ranges::to<std::vector<std::string>>();

                                if(segs.size() < 3) continue;

                                _file_manager->add_group(segs[0], segs[1], segs[2]);
                        }

                        return 1;
                }
                case Protocol::USER_NAME_TAKEN:
                case Protocol::USER_NOT_FOUND:
                case Protocol::WRG_PASSWORD:
                {
                        return static_cast<int>(proto);
                }

                default: break;
        }

        return -100; //unknow error occoured
}


TransportLayer::TransportLayer(int client_fd, std::shared_ptr<bool> login, std::shared_ptr<FileManager> file_manager)
        :
        _client_fd(client_fd),
        _loggedin(login),
        _file_manager(file_manager)
{}

int TransportLayer::create_user(std::string &username, std::string &password) {
        _payload = 
                proto_to_s(Protocol::REGISTER) + " "
                + username + " "
                + password;

        send_payload();
        std::string serv_payload = await_payload();
        int res = resolve_payload(serv_payload);

        if(res == 0) _username = username;
        return res;
}

int TransportLayer::login_user(std::string &username, std::string &password) {
        _payload = 
                proto_to_s(Protocol::LOGIN) + " "
                + username + " " 
                + password;

        send_payload();
        std::string serv_payload = await_payload();
        int res = resolve_payload(serv_payload);

        if(res == 1) _username = username;
        return res;
}

void TransportLayer::send_message(std::string grp_hash, std::string &message) {
        if(message.length() == 0) return;
        // sender + group + message
        _payload = 
                proto_to_s(Protocol::MESSAGE) + " " 
                + _username + " " 
                + grp_hash + " " 
                + message; 

        send_payload();
        return;
}

void TransportLayer::create_group(std::string grp_name, int mem_num, std::vector<std::string> members) { 
        if(mem_num < 1) return;
        if(grp_name.length() == 0) return;
        
        _payload = 
                proto_to_s(Protocol::CREATE_GROUP) + " " 
                + grp_name + " "
                + std::to_string(mem_num); //the user themselves will be added by the server
                                           //as it is preferable to minimise bytes that
                                           //need to be transported
                                
        for(auto &m : members) {
                if(m.size() > 0)
                        _payload += " " + m;
        }

        send_payload();
        return;
}

