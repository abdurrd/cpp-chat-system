#include "SessionHandler.hpp"
#include "FileStore.hpp"
#include <string>

std::unordered_map<Protocal, std::unique_ptr<ProtocalHandler>> SessionHandler::_handlers {
        {Protocal::REGISTER,            std::unique_ptr<RegestrationHandler>()},
        {Protocal::LOGIN,               std::unique_ptr<LoginHandler>()},
        {Protocal::MESSAGE,             std::unique_ptr<MesssageHandler>()},
        {Protocal::CREATE_GROUP,        std::unique_ptr<CreateGroupHandler>()}
};

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

        char buffer[KB];
        while(_running) {
                int valread = recv(client._fd, buffer, KB, 0);
                if(valread == 0) {
                        std::cout << "Log: client #" << client._fd << " disconnected.\n";
                        break;
                }
                buffer[valread] = '\0';

                std::istringstream payload(buffer);

                int protocal;
                payload >> protocal;
                
                if(_handlers.find(static_cast<Protocal>(protocal)) == _handlers.end()) {
                        std::cout << "Recieved non server specific protocal code: " << protocal << "\n";
                        std::cout << "With payload: " << payload.str() << "\n";
                }

                _handlers[static_cast<Protocal>(protocal)]->handle_payload(payload, *this);
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
                                char buf[16];
                                read(user_fd, buf, 16);

                                int new_fd = FileStore::instance().open_new_chat_fd(std::to_string(buf[0]));
                                chat_fd.push_back(new_fd);

                                EV_SET(&event, new_fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, NOTE_EXTEND, 0, nullptr);
                                kevent(kq, &event, 1, nullptr, 0, nullptr);
                        }

                        for(int cfd: chat_fd) {
                                if(triggered[i].ident == static_cast<uintptr_t>(cfd)) {
                                        char buf[KB];
                                        int valread = read(cfd, buf, KB);
                                        buf[valread] = '\0';

                                        std::string payload(buf);
                                        payload = "1 " + std::to_string(cfd_to_hash[cfd]) + " " + payload;

                                        send(client._fd, payload.c_str(), 1000, 0);
                                }
                        }

                }

        }

}
