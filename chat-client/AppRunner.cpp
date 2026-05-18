#include "AppRunner.hpp"

AppRunner::AppRunner(int client_fd)
        : _client_fd(client_fd)
{
        _transport_layer = std::make_shared<TransportLayer>(_client_fd, _loggedIn, _file_manager, _username);
}

void AppRunner::run() {
        //setup
        *_running = true;
        LoginPage login_page(_transport_layer);
        ChatPage chat_page(_transport_layer, _client_fd, _loaded_chats, _username, _loggedIn, _file_manager);

        //render login pade
        login_page.render();

        //create thread for receiver
        std::thread receiver_thread{Reciever{_client_fd, _file_manager, _username, _running}};

        //render chat page
        chat_page.render();

        //exit
        close(_client_fd);
        *_running = false;
        if(receiver_thread.joinable()) receiver_thread.join();
}


