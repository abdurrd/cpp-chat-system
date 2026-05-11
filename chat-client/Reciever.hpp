#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <sys/socket.h>
#include <sstream>
#include <thread>
#include <vector>
#include <ranges>

#include "../Protocol.hpp"
#include "FileManager.hpp"

#define KB 1024

class Reciever {
        int _socket_fd;
        std::shared_ptr<FileManager> _file_manager;

public:
        Reciever(int socket_fd, 
                std::shared_ptr<FileManager> file_manager) 
                : 
                _socket_fd(socket_fd),
                _file_manager(file_manager)
        {}

        void resolver(std::string buffer){
                std::istringstream payload(buffer);

                Protocol proto;
                payload >> proto; 
                std::string data = payload.str().substr(payload.tellg());
                data.substr(data.find_first_not_of(" /t/n"));

                switch(proto) {
                        case Protocol::MESSAGE:
                        {
                                auto segs = data 
                                        | std::views::split(FIELD_SEP)
                                        | std::ranges::to<std::vector<std::string>>();

                                _file_manager->append_message(segs[0], segs[1]);
                                
                                break;
                        }
                        case Protocol::CREATE_GROUP:
                        {
                                auto segs = data
                                        | std::views::split(FIELD_SEP)
                                        | std::ranges::to<std::vector<std::string>>();

                                _file_manager->add_group(segs[0], segs[1]);

                                break;
                        }

                        case Protocol::INVALID_USERS:
                        {
                                //notify with error
                                break;
                        }

                        default:
                        {
                                //notify server request not found
                        }

                }
        }

        void operator()(){
                char buffer[KB];
                for(;;) {
                        int valread = recv(_socket_fd, buffer, KB, 0);
                        if(valread <= 0) {
                                std::cout << " - Connection interupted\n";
                                break;
                        }
                        buffer[valread] = '\0';
                        std::string payload(buffer);

                        resolver(payload);
                }

                return;
        }

};
