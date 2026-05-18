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

#include "Log.hpp"

#include "cpptui.hpp" //external header file from https://github.com/jonoton/cpp-tui.git

using namespace cpptui;

class AppRunner{
        std::shared_ptr<bool> LoggedIn = std::make_shared<bool>(false);
        std::shared_ptr<std::queue<ChatInfo>> loaded_chats = std::make_shared<std::queue<ChatInfo>>();
        ChatInfo focused_chat;
        std::shared_ptr<FileManager> file_manager = std::make_shared<FileManager>(loaded_chats);
        TransportLayer transport_layer;

        int _client_fd;

        std::shared_ptr<bool> _running = std::make_shared<bool>(false);

        Log log{};

public:
        AppRunner(int client_fd);

        void login_page();
        void main_page();
        void run();
};
