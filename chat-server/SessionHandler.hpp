#pragma once
//#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <filesystem>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <fcntl.h>
#include <unordered_map>

#include "../Protocal.hpp"
#include "ProtocalHandeler.hpp"

namespace fs = std::filesystem;

#define KB 1024

class SessionHandler {
private:
        std::atomic<bool> &_running;

        std::thread _watcher_thread; 

        static std::unordered_map<Protocal, std::unique_ptr<ProtocalHandler>> _handlers;

        int _pipefd[2];

        struct Client{
                int _fd;
                std::string _username;

                Client(int fd):_fd{fd}{}
        }client;


public:

        SessionHandler(int clinet_socket, std::atomic<bool> &running);

        const Client& getClient() const;
        void setClientUsername(std::string &username);
        void init_watcher();

        //thread workers
        void operator()();
        void file_watcher();


};

