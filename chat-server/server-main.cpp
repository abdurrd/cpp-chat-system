/**
 * Chat Server - Phase 1: Raw POSIX Sockets
 *
 * TCP server that handles multiple clients using select()
 * Broadcasts messages from one client to all others
 */

//#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
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
#include <mutex>
#include <unistd.h>
#include <fcntl.h>
#include <unordered_map>

#include "../Protocol.hpp"
#include "SessionHandler.hpp"

namespace fs = std::filesystem;

#define PORT 8888
#define MAX_CLIENTS 10

std::atomic<bool> running = true;
void handle_exit(int) { running = false; }

int main() {

        //std::signal(SIGINT, handle_exit);
        
        //varibale declarations
	int             server_fd;                      //file descripter for the socket
        int             new_socket;                     //temperary socket for new connections

        sockaddr_in     address;                        //structure to hold server address information

        int             opt = 1;                            //option for socket configuration

        //creating the socket:
        //AF_INET: IPv4 address family
        //SOCK_STREAM: TCP protocal
        server_fd = socket(AF_INET, SOCK_STREAM, 0);

        if(server_fd == 0)                      perror("socket failed!"), exit(EXIT_FAILURE);
        
        //configuring socket options
        int opt_fail = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if(opt_fail < 0)                        perror("setsockopt failed!"), exit(EXIT_FAILURE);

        //define the address structure
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        //Binding socket
        bool bind_fail = bind(server_fd, (sockaddr*)(&address), sizeof(address)); //retruns 0 on success, -1 on failure 

        if(bind_fail)                           perror("bind failed!"), exit(EXIT_FAILURE);

        //starting to listen
        bool listen_fail = listen(server_fd, 10);

        if(listen_fail)                         perror("listen failed!"), exit(EXIT_FAILURE);

        std::vector<std::thread> client_workers;
        
        //accept incoming clients and delegating them to a thread each
        while(running) {
                new_socket = accept(server_fd, nullptr, nullptr);
                client_workers.push_back(std::thread{SessionHandler(new_socket, running)});
        }
        
        for(auto &w: client_workers) {
                std::cout << "Log: ENDED thread: " << w.get_id() << "\n";
                w.join();
        }
        
	return 0;
}
