#include "Reciever.hpp"

Reciever::Reciever(int socket_fd, 
        std::shared_ptr<FileManager> file_manager, std::shared_ptr<bool> running) 
        : 
        _socket_fd(socket_fd),
        _file_manager(file_manager),
        _running(running)
{}

void Reciever::resolver(std::string buffer){
        std::istringstream payload(buffer);

        Protocol proto;
        payload >> proto; 
        std::string data = payload.str().substr(payload.tellg());
        data = data.substr(data.find_first_not_of(" \t\n"));

        switch(proto) {
                case Protocol::MESSAGE:
                {
                        auto segs = data 
                                | std::views::split(FIELD_SEP)
                                | std::views::filter([](auto s) {
                                        return !s.empty();
                                }) 
                                | std::ranges::to<std::vector<std::string>>();
                        if(segs.size() != 2) break;

                        _file_manager->append_message(segs[0], segs[1]);
                        
                        break;
                }
                case Protocol::CREATE_GROUP:
                {
                        auto segs = data
                                | std::views::split(FIELD_SEP)
                                | std::views::filter([](auto s) {
                                        return !s.empty();
                                }) 
                                | std::ranges::to<std::vector<std::string>>();

                        for(auto seg: segs) Log()("seg from Reciever::CREATE_GROUP", seg);

                        if(segs.size() != 2) break;

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

void Reciever::operator()(){
        char buffer[BUFFER_SIZE];
        while(_running) {
                int valread = recv(_socket_fd, buffer, BUFFER_SIZE, 0);
                if(valread <= 0) {
                        std::cout << " - Connection interupted\n";
                        break;
                }
                buffer[valread] = '\0';
                std::string payload(buffer);

                resolver(payload);
        }
        close(_socket_fd);

        return;
}
