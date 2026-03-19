/**
 * Chat Client - Phase 1: Raw POSIX Sockets
 *
 * TCP client that connects to server
 * Uses threading for simultaneous send/receive
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8888
#define BUFFER_SIZE 1024

// TODO: Implement receiver thread function
void *receive_messages(void *socket_descriptor){
        int sock = *(static_cast<int*> (socket_descriptor));
        char buffer[BUFFER_SIZE];

        for(;;) {
                int valread = read(sock, buffer, BUFFER_SIZE);
                if(valread <= 0) break;
                buffer[valread] = '\0';
                std::cout << "received: " << buffer << "\n";
        }

        return nullptr;
}

// TODO: Implement main function

int main() {


	return 0;
}
