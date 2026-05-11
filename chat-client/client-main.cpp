#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

#include "AppRunner.hpp"

int main() {

        //Main thread: Handles user input and sends messages
        //Secondary thread: Continuously listens for incoming messages
        //This allows for real-time bidirectional communication
        
        int             sock {0};               //client socket descriptor
        sockaddr_in     serv_addr;              //server address structure

        //creating client socket
        sock = socket(AF_INET, SOCK_STREAM, 0);

        if(sock < 0)            perror("socket creation failed!"), exit(EXIT_FAILURE);

        //setting up server address
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        
        char serv_ip[] = "127.0.0.1";
        bool invalid_address = inet_pton(AF_INET, serv_ip, &serv_addr.sin_addr) <= 0;

        if(invalid_address)     perror("invalid address!"), exit(EXIT_FAILURE);

        //establish connection with server
        bool connect_fail = connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));

        if(connect_fail)        perror("connect failed!"), exit(EXIT_FAILURE);

        std::cout << "connected to server on: " << PORT << "\n";

        //init app
        AppRunner app(sock);
        app.run();
        
	return 0;
}
