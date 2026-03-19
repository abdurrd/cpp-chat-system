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

#define PORT 8888
#define BUFFER_SIZE 1024


//receiver thread function
void *receive_messages(void *socket_descriptor){
        int sock = *(static_cast<int*> (socket_descriptor));
        char buffer[BUFFER_SIZE];
                                                                                
        for(;;) {
                int valread = read(sock, buffer, BUFFER_SIZE);
                if(valread <= 0) {
                        std::cout << " - Connection interupted\n";
                        break;
                }
                buffer[valread] = '\0';
                std::cout << "received: " << buffer << "\n";
        }

        return nullptr;
}

int main() {

        //Main thread: Handles user input and sends messages
        //Secondary thread: Continuously listens for incoming messages
        //This allows for real-time bidirectional communication
        
        int             sock {0};               //client socket descriptor
        sockaddr_in     serv_addr;              //server address structure
        char            buffer[BUFFER_SIZE];    //message buffer
        pthread_t       thread_id;              //thread identifier for receive thread

        //creating client socket
        sock = socket(AF_INET, SOCK_STREAM, 0);

        if(sock < 0)            perror("socket creation failed!"), exit(EXIT_FAILURE);

        //setting up server address
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        bool invalid_address = inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0;

        if(invalid_address)     perror("invalid address!"), exit(EXIT_FAILURE);

        //establish connection with server
        bool connect_fail = connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));

        if(connect_fail)        perror("connect failed!"), exit(EXIT_FAILURE);

        std::cout << "connected to server on: " << PORT << "\n";
        
        //create thread for receiver
        bool thread_err = pthread_create(&thread_id, nullptr, receive_messages, static_cast<void*>(&sock)) < 0;

        if(thread_err)          perror("thread creation failed!"), exit(EXIT_FAILURE);

        //main sending loop
        for(;;) {
                std::cout << "Message: ";
                fgets(buffer, BUFFER_SIZE, stdin);
                send(sock, buffer, strlen(buffer), 0);
        }
        
	return 0;
}
