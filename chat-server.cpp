/**
 * Chat Server - Phase 1: Raw POSIX Sockets
 *
 * TCP server that handles multiple clients using select()
 * Broadcasts messages from one client to all others
 */

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <filesystem>

#include "Protocal.hpp"

namespace fs = std::filesystem;

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

std::string read_to_eof(std::string &&file_path){
        std::ifstream file_data(file_path); //might optomise this later
        std::stringstream file_contents;                                  
        file_contents << file_data.rdbuf();

        return file_contents.str();
}

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

        //define the address structure
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
                        
                        if(new_socket < 0)      perror("accept failed!"), exit(EXIT_FAILURE);
                        
                        for(int i {}; i < MAX_CLIENTS; ++i){
                                if(client_sockets[i] != 0) continue;

                                client_sockets[i] = new_socket;
                                std::cout << "New client:" << client_sockets[i] << " connected.\n";

                                break;
                        }

                }
                
                //handling client messages this entire thing will be delegated to a thread of its own
                //this thread will mearly handle all the connection requests and will delegate this 
                //protocal loop for each new connection and each thread will have its own handlers
                //also since multiple thrads will try to write to the same files we must use mutexes.
                for(int i {}; i < MAX_CLIENTS; ++i){
                        if(client_sockets[i] <= 0) continue;
                        if(!FD_ISSET(client_sockets[i], &readfds)) continue;

                        int valread = recv(client_sockets[i], buffer, BUFFER_SIZE, 0);

                        if(valread == 0) {
                                close(client_sockets[i]);
                                std::cout << "client:" << client_sockets[i] << " disconnected\n";
                                client_sockets[i] = 0;              
                                continue;
                        }

                        buffer[valread] = '\0';
                        std::stringstream payload(buffer);

                        int protocal;
                        payload >> protocal;

                        switch(static_cast<Protocal>(protocal)) {
                                case Protocal::REGISTER:
                                {
                                        std::string username, password;
                                        payload >> username >> password;

                                        fs::path user_path = "./server-data/users/" + username + ".txt";
                                        if(fs::exists(user_path)) {
                                                char err_message[] = "-1 Username_taken\0";
                                                send(client_sockets[i], err_message, strlen(err_message), 0);
                                                break;
                                        }

                                        std::ofstream new_user(user_path);
                                        new_user << password << "\n";

                                        char success_message[] = "1 User_created!\0";
                                        send(client_sockets[i], success_message, strlen(success_message), 0);
                                        break;
                                }
                                case Protocal::LOGIN:
                                {
                                        std::string u_username, u_password;
                                        payload >> u_username >> u_password;

                                        fs::path user_path = "./server-data/users/" + u_username + ".txt";
                                        if(!fs::exists(user_path)) {
                                                char err_message[] = "-1 Username_not_found\0";
                                                send(client_sockets[i], err_message, strlen(err_message), 0);
                                                break;
                                        }

                                        std::ifstream user_data(user_path);
                                        std::string password;
                                        user_data >> password;
                                        if(u_password != password) {
                                                char err_message[] = "-1 Wrong_password\0";
                                                send(client_sockets[i], err_message, strlen(err_message), 0);
                                                break;
                                        }

                                        int group_no = -1;
                                        std::string group_data;
                                        std::string line;
                                        while(std::getline(user_data, line)) {
                                                group_data += line;
                                                group_data += '\n';

                                                group_data += read_to_eof("./server-data/groups/"+line+".txt");
                                                group_data += read_to_eof("./server-data/chats/"+line+".txt");

                                                ++group_no;
                                        }
                                        group_data = std::to_string(group_no) + '\n' + group_data;
                                        group_data += '\0';
                                        send(client_sockets[i], group_data.c_str(), strlen(group_data.c_str()), 0);
                                        break;
                                }
                                case Protocal::MESSAGE:
                                {
                                        std::string sender, group, message;
                                        payload >> sender >> group;
                                        std::getline(payload, message);
                                        fs::path chat_path("./server-data/chats/" + group + ".txt");

                                        //no need to handle braodcast as this will update the chat file
                                        //which will be being watched by all other live memebers on their
                                        //own thread
                                        
                                        std::ofstream chat_out(chat_path, std::ios::app);
                                        chat_out << sender << " ";
                                        chat_out << message << "\n";

                                        break;
                                }
                                case Protocal::CREATE_GROUP:
                                {
                                        std::cout << "Create group protocal\n";
                                        break;
                                }
                                default:
                                {
                                        std::cout << "Recieved non server specific protocal code: " << protocal << "\n";
                                        std::cout << "With payload: " << payload.str() << "\n";

                                }
                        }

                       // for(int j {}; j < MAX_CLIENTS; ++j){
                       //         if(i == j || client_sockets[j] <= 0) continue; //all connected clients except i-th/current client
                       //         send(client_sockets[j], buffer, strlen(buffer), 0);
                       // }
                }

        }
        


        


	return 0;
}
