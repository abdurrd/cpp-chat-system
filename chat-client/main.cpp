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

Client client_handler;

struct Message {
        std::string _content;
        User _sender;
};

int main() {

        //Main thread: Handles user input and sends messages
        //Secondary thread: Continuously listens for incoming messages
        //This allows for real-time bidirectional communication

	return 0;
}


