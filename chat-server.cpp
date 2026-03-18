/**
 * Chat Server - Phase 1: Raw POSIX Sockets
 * 
 * TCP server that handles multiple clients using select()
 * Broadcasts messages from one client to all others
 */

#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    // TODO: Implement server logic
    
    return 0;
}
