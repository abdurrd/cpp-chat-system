#pragma once
//#include <csignal>
#include <cstdlib>
#include <memory>
#include <cstring>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <atomic>
#include <unistd.h>
#include <fcntl.h>
#include <unordered_map>

#include "../Protocol.hpp"
#include "ProtocolHandeler.hpp"

class SessionHandler {
private:
        std::thread _watcher_thread; 

        static std::unordered_map<Protocol, std::unique_ptr<ProtocolHandler>> _handlers;

        int _pipefd[2];

        struct Client{
                int _fd;
                std::string _username;

                Client(int fd):_fd{fd}{}
        }client;

        std::atomic<bool> &_running;

public:

        SessionHandler(int clinet_socket, std::atomic<bool> &running);

        const Client& getClient() const;
        void setClientUsername(std::string &username);
        void init_watcher();

        //thread workers
        void operator()();
        void file_watcher();


};

