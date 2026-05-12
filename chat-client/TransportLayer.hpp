#pragma once

#include "../Protocol.hpp"
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <vector>
#include <memory>
#include <sstream>
#include <ranges>

#include "FileManager.hpp"

class TransportLayer {
        std::string _username;
        int _client_fd;
        std::string _payload;
        std::shared_ptr<bool> _loggedin;
        std::shared_ptr<FileManager> _file_manager;

        std::string proto_to_s(Protocol proto);
        void send_payload();
        std::string await_payload();
        int resolve_payload(std::string serv_payload);

public:

        TransportLayer() = default;
        TransportLayer(int client_fd, std::shared_ptr<bool> login, std::shared_ptr<FileManager> file_manager);
        
        int create_user(std::string &username, std::string &password);
        int login_user(std::string &username, std::string &password);
        void send_message(std::string grp_hash, std::string &message);
        void create_group(std::string grp_name, int mem_num, std::vector<std::string> members);
};
