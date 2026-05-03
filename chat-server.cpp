/**
 * Chat Server - Phase 1: Raw POSIX Sockets
 *
 * TCP server that handles multiple clients using select()
 * Broadcasts messages from one client to all others
 */

//#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <filesystem>
#include <vector>
#include <atomic>
#include <mutex>
#include <unistd.h>
#include <fcntl.h>
#include <unordered_map>

#include "Protocal.hpp"

namespace fs = std::filesystem;

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

std::atomic<bool> running = true;
void handle_exit(int) { running = false; }

std::mutex user_mutex;
std::mutex group_mutex;
std::mutex chat_mutex;

std::string read_to_eof(std::string &&file_path){
        std::ifstream file_data(file_path); //might optomise this later
        std::stringstream file_contents;                                  
        file_contents << file_data.rdbuf();

        return file_contents.str();
}

void* file_watcher(void *client_socket, int pipe_read_end, std::string username) {
        int client = *(int*)client_socket;
        
        fs::path user_path = "./server-data/users/" + username + ".txt";
        
        int user_fd = open(user_path.c_str(), O_RDONLY);
        lseek(user_fd, 0, SEEK_END);

        std::vector<int> chat_hashs;

        {//just to discard
                std::ifstream user_file(user_path);

                std::string discard;
                std::getline(user_file, discard);

                std::string hash;
                while(std::getline(user_file, hash)) {
                        chat_hashs.push_back(std::stoi(hash));
                }
        }

        std::vector<int> chat_fd;
        std::unordered_map<int,int> cfd_to_hash;
        for(int hash: chat_hashs) {
                std::string chat_path = "./server-data/chats/" + std::to_string(hash) + ".txt";
                chat_fd.push_back(open(chat_path.c_str(), O_RDONLY));
                cfd_to_hash[*--chat_fd.end()] = hash;
        }
        
        for(int cfd: chat_fd) {
                lseek(cfd, 0, SEEK_END);
        }

        int kq = kqueue();

        struct kevent event;

        EV_SET(&event, pipe_read_end, EVFILT_READ, EV_ADD, 0, 0, nullptr);
        kevent(kq, &event, 1, nullptr, 0, nullptr);

        EV_SET(&event, user_fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, NOTE_EXTEND, 0, nullptr);
        kevent(kq, &event, 1, nullptr, 0, nullptr);

        for(int cfd: chat_fd) {
                EV_SET(&event, cfd, EVFILT_VNODE, EV_ADD | EV_CLEAR, NOTE_EXTEND, 0, nullptr);
                kevent(kq, &event, 1, nullptr, 0, nullptr);
        }

        struct kevent triggered[16];
        while(true) {
                int n = kevent(kq, nullptr, 0, triggered, 16, nullptr);

                if(n < 0) { perror("bad kevent\n"); break; }

                for(int i = 0; i < n; ++i) {
                        if(triggered[i].ident == static_cast<uintptr_t>(pipe_read_end)) {
                                break;
                        }
                
                        if(triggered[i].ident == static_cast<uintptr_t>(user_fd)) {
                                char buf[16];
                                read(user_fd, buf, 16);

                                std::string file = "./server-data/chats/" + std::to_string(buf[0]) + ".txt";
                                int new_fd = open(file.c_str(), O_RDONLY);
                                chat_fd.push_back(new_fd);

                                EV_SET(&event, new_fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, NOTE_EXTEND, 0, nullptr);
                                kevent(kq, &event, 1, nullptr, 0, nullptr);
                        }

                        for(int cfd: chat_fd) {
                                if(triggered[i].ident == static_cast<uintptr_t>(cfd)) {
                                        char buf[1000];
                                        int valread = read(cfd, buf, 1000);
                                        buf[valread] = '\0';

                                        std::string payload(buf);
                                        payload = "1 " + std::to_string(cfd_to_hash[cfd]) + " " + payload;

                                        send(client, payload.c_str(), 1000, 0);
                                }
                        }

                }

        }

        return nullptr;
}

void* client_handler(void *client_socket) {
        int client = *(int*)client_socket;
        std::cout << "Log: client #" << client << " on thread: " << std::this_thread::get_id() << "\n";

        int pipefd[2];
        pipe(pipefd);

        std::thread watcher_thread; 

        char buffer[BUFFER_SIZE];
        while(running) {
                int valread = recv(client, buffer, BUFFER_SIZE, 0);
                if(valread == 0) {
                        std::cout << "Log: client #" << client << " disconnected.\n";
                        break;
                }
                // process protocals
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
                                        send(client, err_message, strlen(err_message), 0);
                                        break;
                                }

                                {// useer lock
                                        std::lock_guard<std::mutex> lock(user_mutex);
                                        std::ofstream new_user(user_path);
                                        new_user << password << "\n";

                                }

                                char success_message[] = "1 User_created!\0";
                                send(client, success_message, strlen(success_message), 0);
                                break;
                        }
                        case Protocal::LOGIN:
                        {
                                std::string u_username, u_password;
                                payload >> u_username >> u_password;

                                fs::path user_path = "./server-data/users/" + u_username + ".txt";
                                if(!fs::exists(user_path)) {
                                        char err_message[] = "-1 Username_not_found\0";
                                        send(client, err_message, strlen(err_message), 0);
                                        break;
                                }
                                
                                std::string password;

                                std::ifstream user_data(user_path);
                                {// user lock
                                        std::lock_guard<std::mutex> lock(user_mutex);
                                        user_data >> password;
                                }

                                if(u_password != password) {
                                        char err_message[] = "-1 Wrong_password\0";
                                        send(client, err_message, strlen(err_message), 0);
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
                                send(client, group_data.c_str(), strlen(group_data.c_str()), 0);
                                watcher_thread = std::thread(file_watcher, client_socket, pipefd[0], u_username);
                                break;
                        }
                        case Protocal::MESSAGE:
                        {
                                std::string sender, group, message;
                                payload >> sender >> group;
                                std::getline(payload, message);
                                fs::path chat_path("./server-data/chats/" + group + ".txt");

                                //no need to handle braodcast as this will update the chat file
                                //which will be watched by all other live memebers on their
                                //own thread
                                
                                std::ofstream chat_out(chat_path, std::ios::app);
                                chat_out << sender << " ";
                                chat_out << message << "\n";
                                
                                break;
                        }
                        case Protocal::CREATE_GROUP:
                        {
                                std::cout << "create group protocal: " << payload.str() << "\n";
                                std::string grp_name;
                                int num_members;
                                std::vector<std::string> users;

                                payload >> grp_name;
                                payload >> num_members;
                                users = std::vector<std::string>(num_members);
                                for(int i = 0; i < num_members; ++i) {
                                        payload >> users[i];
                                }

                                for(std::string user: users) {
                                        // add validation and update user files to include groups
                                }
                                
                                fs::path gh_path = "./server-data/groups/hash_key.txt";
                                std::fstream gh_file(gh_path, std::ios::in | std::ios::out);

                                int group_hash;
                                gh_file >> group_hash;
                                ++group_hash;

                                fs::path new_grp_path = "./server-data/groups/" + std::to_string(group_hash) + ".txt";

                                std::ofstream new_grp_file(new_grp_path);
                                
                                new_grp_file << grp_name << "\n";
                                for(int i = 0; i < num_members; ++i) {
                                        new_grp_file << users[i] << "\n";
                                }
                                std::cout << group_hash << "\n";
                                gh_file.seekp(0);
                                gh_file << group_hash;

                                break;
                        }
                        default:
                        {
                                std::cout << "Recieved non server specific protocal code: " << protocal << "\n";
                                std::cout << "With payload: " << payload.str() << "\n";

                        }
                }
        }

        write(pipefd[1], "x", 1);
        if(watcher_thread.joinable()) watcher_thread.join();
        close(client);

        return nullptr;
}


int main() {

        //std::signal(SIGINT, handle_exit);
        
        //varibale declarations
	int             server_fd;                      //file descripter for the socket
        int             new_socket;                     //temperary socket for new connections

        sockaddr_in     address;                        //structure to hold server address information

        int             opt = 1;                            //option for socket configuration

        //creating the socket:
        //AF_INET: IPv4 address family
        //SOCK_STREAM: TCP protocal
        server_fd = socket(AF_INET, SOCK_STREAM, 0);

        if(server_fd == 0)                      perror("socket failed!"), exit(EXIT_FAILURE);
        
        //configuring socket options
        int opt_fail = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if(opt_fail < 0)                        perror("setsockopt failed!"), exit(EXIT_FAILURE);

        //define the address structure
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        //Binding socket
        bool bind_fail = bind(server_fd, (sockaddr*)(&address), sizeof(address)); //retruns 0 on success, -1 on failure 

        if(bind_fail)                           perror("bind failed!"), exit(EXIT_FAILURE);

        //starting to listen
        bool listen_fail = listen(server_fd, 10);

        if(listen_fail)                         perror("listen failed!"), exit(EXIT_FAILURE);

        std::vector<std::thread> client_workers;
        
        //accept incoming clients and delegating them to a thread each
        while(running) {
                new_socket = accept(server_fd, nullptr, nullptr);
                client_workers.push_back(std::thread{client_handler, &new_socket});
        }
        
        for(auto &w: client_workers) {
                std::cout << "Log: ENDED thread: " << w.get_id() << "\n";
                w.join();
        }
        
	return 0;
}
