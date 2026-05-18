#pragma once

#include <algorithm>
#include <cctype>
#include <memory.h>
#include <memory>
#include <queue>
#include <ranges>
#include <unistd.h>

#include "ChatInfo.hpp"
#include "FileManager.hpp"
#include "Reciever.hpp"
#include "TransportLayer.hpp"

#include "LoginPage.hpp"
#include "ChatPage.hpp"


#include "cpptui.hpp" //external header file from https://github.com/jonoton/cpp-tui.git


class AppRunner {
        int _client_fd;

        std::shared_ptr<std::queue<ChatInfo>> _loaded_chats = std::make_shared<std::queue<ChatInfo>>();
        std::shared_ptr<FileManager> _file_manager = std::make_shared<FileManager>(_loaded_chats);
        std::shared_ptr<TransportLayer> _transport_layer;

        std::shared_ptr<bool> _loggedIn = std::make_shared<bool>(false);
        std::shared_ptr<std::string> _username = std::make_shared<std::string>("");

        std::shared_ptr<bool> _running = std::make_shared<bool>(false);

public:
        AppRunner(int client_fd);
        void run();
};

