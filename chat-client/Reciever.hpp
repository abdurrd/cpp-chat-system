#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <sys/socket.h>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <vector>
#include <ranges>

#include "../Protocol.hpp"
#include "FileManager.hpp"

#include "Log.hpp"

#define BUFFER_SIZE 64*1024

class Reciever {
        int _socket_fd;
        std::shared_ptr<FileManager> _file_manager;

        std::shared_ptr<std::string> _username;

        std::shared_ptr<bool> _running;
public:
        Reciever(int socket_fd, std::shared_ptr<FileManager> file_manager, std::shared_ptr<std::string> user, std::shared_ptr<bool> running);

        void resolver(std::string buffer);
        void operator()();
};
