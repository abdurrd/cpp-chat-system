#pragma once
#include "Page.hpp"
#include "ChatInfo.hpp"
#include "FileManager.hpp"

class ChatPage : public Page {
        int _client_fd;

        ChatInfo focused_chat;

        std::shared_ptr<std::queue<ChatInfo>> _loaded_chats;

        std::shared_ptr<std::string> _username;
        std::shared_ptr<bool> _loggedIn;
        
        std::shared_ptr<bool> _running = std::make_shared<bool>(false);

        std::shared_ptr<FileManager> _file_manager;

public:
        ChatPage(std::shared_ptr<TransportLayer> transport_layer, 
                int client, std::shared_ptr<std::queue<ChatInfo>> chat_queue, 
                std::shared_ptr<std::string> user, 
                std::shared_ptr<bool> loggedin, 
                std::shared_ptr<FileManager> file_manager);

        void construct() override;
};
