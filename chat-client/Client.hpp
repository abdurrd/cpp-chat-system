#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define KB 1024

#define ERR(x) perror(x), exit(EXIT_FAILURE)

class Client {
private:
        int             _sock {0};               //client socket descriptor
        sockaddr_in     _serv_addr;              //server address structure
        pthread_t       _thread_id;              //thread identifier for receive thread
        char            _buffer[BUFFER_SIZE];

        enum code : uint8_t {
                SIGNUP,
                LOGIN,
                MESSAGE,
        };
public:

        void connect_to_server();
        static void *receive_messages(void *socket_descriptor);

        bool login(std::string name, std::string password); 
        void chat();
};
