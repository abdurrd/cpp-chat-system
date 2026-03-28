/**
 * Chat Client - Phase 1: Raw POSIX Sockets
 *
 * TCP client that connects to server
 * Uses threading for simultaneous send/receive
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "Client.hpp"
#include "User.hpp"

#define ERR(x) perror(x), exit(EXIT_FAILURE)

//USE STRING STREAMS!!!!
//Main thread: Handles user input and sends messages
//Secondary thread: Continuously listens for incoming messages
//This allows for real-time bidirectional communication

struct Message {
        std::string     _content;
        User            _sender;
};

int main() {
        
        User    user("alex", "123");
        Client  client;

        client.connect_to_server();
        
        bool login_val = client.login_user(user); if(!login_val) ERR("Login in failed");

        client.chat(user);

	return 0;
}


