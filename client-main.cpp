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
#define KB 1024

#define ERR(x) perror(x), exit(EXIT_FAILURE)

//USE STRING STREAMS!!!!

struct Client {

        int             _sock {0};               //client socket descriptor
        sockaddr_in     _serv_addr;              //server address structure
        pthread_t       _thread_id;              //thread identifier for receive thread
        char            _buffer[BUFFER_SIZE];

        void connect_to_server(){
                //creating client socket
                _sock = socket(AF_INET, SOCK_STREAM, 0);

                if(_sock < 0) ERR("socket creation failed!");

                //setting up server address
                _serv_addr.sin_family = AF_INET;
                _serv_addr.sin_port = htons(PORT);

                bool invalid_address = inet_pton(AF_INET, "127.0.0.1", &_serv_addr.sin_addr) <= 0;

                if(invalid_address) ERR("invalid address!");

                //establish connection with server
                bool connect_fail = connect(_sock, (sockaddr*)&_serv_addr, sizeof(_serv_addr));

                if(connect_fail) ERR("connect failed!");

                std::cout << "connected to server on: " << PORT << "\n";
        }

        //receiver thread function
        static void *receive_messages(void *socket_descriptor);

        void chat() {
                //create thread for receiver
                bool thread_err = pthread_create(&_thread_id, nullptr, receive_messages, static_cast<void*>(&_sock)) < 0;

                if(thread_err) ERR("thread creation failed!");

                //main sending loop
                for(;;) {
                        std::cout << "Message: ";
                        fgets(_buffer, BUFFER_SIZE, stdin);
                        send(_sock, _buffer, strlen(_buffer), 0);
                }
        }

        enum code {
                SIGNUP,
                LOGIN,
                MESSAGE,
        };

        bool login(std::string name, std::string password) {
                std::string s_buffer = std::to_string(LOGIN) + name + ' ' + password;
                send(_sock, s_buffer.c_str(), s_buffer.length(), 0);

                char res_buf[KB];
                int val_read = read(_sock, res_buf, strlen(res_buf));
                if(val_read <= 0){
                        std::cerr << "Issue receiving login confirmation\n";
                        return false;
                }

                return res_buf[0] == 'T';
        }

} client_handler;

struct User {
        std::string _name;

        User(std::string name) : _name(name) {}

        std::string get_name() {return _name;}

        User login(){
                std::string name;
                std::cout << "Username: ";
                std::cin >> name;
                std::cout << "\n";

                std::string password;
                std::cout << "Password: ";
                std::cin >> password;
                std::cout << "\n";

                bool auth = client_handler.login(name, password);
                if(!auth) ERR("Login in failed");

                User user(name);

                return user;
        }

        bool signup();
};

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
                std::cout << buffer << "\n";
        }

        return nullptr;
}
