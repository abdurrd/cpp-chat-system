#include "SessionHandler.hpp"
#include "FileStore.hpp"
#include <string>

std::unordered_map<Protocol, std::unique_ptr<ProtocolHandler>> SessionHandler::_handlers = []{
        std::unordered_map<Protocol, std::unique_ptr<ProtocolHandler>> m;

        m.emplace(Protocol::REGISTER,            std::make_unique<RegestrationHandler>());
        m.emplace(Protocol::LOGIN,               std::make_unique<LoginHandler>());
        m.emplace(Protocol::MESSAGE,             std::make_unique<MesssageHandler>());
        m.emplace(Protocol::CREATE_GROUP,        std::make_unique<CreateGroupHandler>());

        return m;
}();

SessionHandler::SessionHandler(int clinet_socket, std::atomic<bool> &running)
                : 
        client{clinet_socket}, 
        _running(running) 
        {pipe(_pipefd);}


const SessionHandler::Client& SessionHandler::getClient() const {
        return client;
}

void SessionHandler::setClientUsername(std::string &username) {
        client._username = username;
}

void SessionHandler::init_watcher() {
        _watcher_thread = std::thread([this] {
                file_watcher();
        });
}

void SessionHandler::operator()() {
        std::cout << "Log: client #" << client._fd << " on thread: " << std::this_thread::get_id() << "\n";

        char buffer[BUFFER_SIZE];
        while(_running) {
                int valread = recv(client._fd, buffer, BUFFER_SIZE, 0);
                if(valread == 0) {
                        std::cout << "Log: client #" << client._fd << " disconnected.\n";
                        break;
                }
                buffer[valread] = '\0';

                std::istringstream payload(buffer);

                Protocol protocol;
                payload >> protocol;
                
                if(_handlers.find(protocol)== _handlers.end()) {
                        std::cout << "Recieved non server specific protocol: " << protocol << "\n";
                        std::cout << "With payload: " << payload.str() << "\n";
                } 
                else _handlers[protocol]->handle_payload(payload, *this);

        }

        write(_pipefd[1], "x", 1);
        if(_watcher_thread.joinable()) _watcher_thread.join();
        close(client._fd);
}


void SessionHandler::file_watcher() {
        
        int user_fd = FileStore::instance().open_user_fd(client._username);

        std::vector<int> chat_fd;
        std::unordered_map<int,int> cfd_to_hash;
        FileStore::instance().open_chat_fds(client._username, chat_fd, cfd_to_hash);

        int kq = kqueue();

        struct kevent event;

        EV_SET(&event, _pipefd[0], EVFILT_READ, EV_ADD, 0, 0, nullptr);
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
                        if(triggered[i].ident == static_cast<uintptr_t>(_pipefd[0])) {
                                break;
                        }
                
                        if(triggered[i].ident == static_cast<uintptr_t>(user_fd)) {
                                std::unique_lock<std::mutex> user_lock(FileStore::instance().get_user_mutex());
                                char buf[8];
                                read(user_fd, buf, 8);
                                user_lock.unlock();

                                std::string hs = buf;
                                hs.erase(hs.find_last_not_of(" \t\n\r") + 1);
                                int new_fd = FileStore::instance().open_new_chat_fd(hs);
                                chat_fd.push_back(new_fd);
                                cfd_to_hash[new_fd] = std::stoi(hs);

                                EV_SET(&event, new_fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, NOTE_EXTEND, 0, nullptr);
                                kevent(kq, &event, 1, nullptr, 0, nullptr);

                                std::string payload = "3\n";
                                FileStore::instance().copy_group_data(hs, payload);
                                std::cout << "file watcher: \n" << payload << "\n";
                                send(client._fd, payload.c_str(), strlen(payload.c_str()), 0);
                        }

                        for(int cfd: chat_fd) {
                                if(triggered[i].ident == static_cast<uintptr_t>(cfd)) {
                                        char buf[BUFFER_SIZE];
                                        int valread = read(cfd, buf, BUFFER_SIZE);
                                        buf[valread] = '\0';

                                        std::string payload;
                                        payload = "2\n";
                                        payload += std::to_string(cfd_to_hash[cfd]) + FIELD_SEP + buf;
                                        std::cout << "watcher message payload: "<< payload << "\n";

                                        send(client._fd, payload.c_str(), strlen(payload.c_str()), 0);
                                }
                        }

                }

        }

}
