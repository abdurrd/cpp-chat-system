#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sstream>
#include <thread>
#include <vector>
#include <ranges>

#include "../Protocol.hpp"
#define KB 1024

class Reciever {
        //unordered_map<int, std::unique_ptr<FileManager>> managers; 
public:
        static void resolver(std::string buffer){
                std::istringstream payload(buffer);

                Protocol proto;
                payload >> proto; 

                switch(proto) {
                        case Protocol::REGISTER:
                        {

                        }
                        case Protocol::LOGIN:
                        {
                                int group_no;
                                payload >> group_no;
                                int group_hash;
                                std::string group_name;
                                std::string members_names;
                                std::string chat_history;
                                std::vector<std::string> members;
                                while(group_no--) {
                                        payload >> group_hash;
                                        payload >> group_name;
                                        std::getline(payload, members_names, '%');
                                        std::getline(payload, chat_history, '%');

                                        members = members_names 
                                                | std::views::split('\n')
                                                | std::ranges::to<std::vector<std::string>>();

                                        //std::istringstream buf(members_names);
                                        //std::string member;
                                        //while(getline(buf, member)){
                                        //       members.push_back(member);
                                        //}

                                        //

                                        members.clear();
                                }
                                break;
                        }
                        case Protocol::MESSAGE:
                        {
                                
                        }
                        case Protocol::CREATE_GROUP:
                        {

                        }
                }
        }

        void operator()(int server_fd){
                char buffer[KB];
                for(;;) {
                        int valread = recv(server_fd, buffer, KB, 0);
                        if(valread <= 0) {
                                std::cout << " - Connection interupted\n";
                                break;
                        }
                        buffer[valread] = '\0';
                        std::string payload(buffer);
                        std::thread de_thread(resolver,payload);
                        de_thread.detach();
                }

                return;
        }

};
