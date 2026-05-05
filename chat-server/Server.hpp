#include <csignal>
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

#include "DataHandler.hpp"

class Server {
	int _server_fd;                      //file descripter for the socket
        int _new_socket;                     //temperary socket for new connections

        sockaddr_in _address;                        //structure to hold server address information

        int _opt = 1;                            //option for socket configuration
        
        DataHandler data_handler;

        std::vector<std::thread> client_workers;
        void* client_worker;
        void* file_watcher;
public:
        Server();
        void acceptConnections();
        
};
