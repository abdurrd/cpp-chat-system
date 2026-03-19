/**
 * Chat Server - Phase 1: Raw POSIX Sockets
 *
 * TCP server that handles multiple clients using select()
 * Broadcasts messages from one client to all others
 */

#include <cstdio>
#include <cstdlib>
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
        //varibale declarations
	int             server_fd;                      //file descripter for the socket
        int             new_socket;                     //temperary socket for new connections
        int             client_sockets[MAX_CLIENTS] {}; //array to store all client socket descripters

        sockaddr_in     address;                        //structure to hold server address information

        int             opt;                            //option for socket configuration
        int             max_sd;                         //highest socket descriptor number
        int             activity;                       //return value from select()

        fd_set          readfds;                        //file descriptor set for monitoring

        char            buffer[BUFFER_SIZE] {0};        //buffer to store incoming messages

        //creating the socket:
        //AF_INET: IPv4 address family
        //SOCK_STREAM: TCP protocal
        server_fd = socket(AF_INET, SOCK_STREAM, 0);

        if(server_fd == 0)                      perror("socket failed!"), exit(EXIT_FAILURE);
        
        //configuring socket options
        bool opt_fail = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if(opt_fail)                            perror("setsockopt failed!"), exit(EXIT_FAILURE);

        //setup the address structure
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        //Binding socket
        bool bind_fail = bind(server_fd, (sockaddr*)(&address), sizeof(address)); //retruns 0 on success, -1 on failure 

        if(bind_fail)                           perror("bind failed!"), exit(EXIT_FAILURE);

        //starting to listen
        bool listen_fail = listen(server_fd, 3);

        if(listen_fail)                         perror("listen failed!"), exit(EXIT_FAILURE);

        //server loop
        for(;;) {
                FD_ZERO(&readfds);              //clear all file descripters
                FD_SET(server_fd, &readfds);    //add server socket to set
                max_sd = server_fd;

                //add all clients sockets to set
                for(int i {}; i < MAX_CLIENTS; ++i) {
                        if(client_sockets[i] <= 0) continue;

                        FD_SET(client_sockets[i], &readfds);
                        
                        //need max discripter size for select()
                        max_sd = std::max(max_sd, client_sockets[i]);
                }

                activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);

                if(activity < 0)                perror("select failed!");
                
                //handling new client connections
                if(FD_ISSET(server_fd, &readfds)) {
                        new_socket = accept(server_fd, nullptr, nullptr);
                        
                        if(new_socket < 0)      perror("accept failed!"); exit(EXIT_FAILURE);
                        
                        for(int i {}; i < MAX_CLIENTS; ++i){
                                if(client_sockets[i] != 0) continue;

                                client_sockets[i] = new_socket;
                                std::cout << "New client:" << client_sockets[i] << " connected.\n"];

                                break;
                        }

                }
                
                //handling client messages
                for(int i {}; i < MAX_CLIENTS; ++i){
                        if(client_sockets[i] <= 0) continue;
                        if(!FD_ISSET(client_sockets[i], &readfds)) continue;

                        int valread = read(client_sockets[i], buffer, BUFFER_SIZE);

                        if(valread == 0) {
                                close(client_sockets[i]);
                                std::cout << "client:" << client_sockets[i] << " disconnected\n";
                                client_sockets[i] = 0;              
                                continue;
                        }

                        buffer[valread] = '\0';
                        for(int j {}; i < MAX_CLIENTS; ++j){
                                if(i == j || client_sockets[j] <= 0) continue; //all connected clients except i-th/current client
                                send(client_sockets[i], buffer, strlen(buffer), 0);
                        }
                }

        }
        


        


	return 0;
}
