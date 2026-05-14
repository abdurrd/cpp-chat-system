#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <mutex>
#include <queue>

#include "ChatInfo.hpp"

#include "Log.hpp"

namespace fs = std::filesystem;

class FileManager {
        inline static std::mutex _group_mutex;
        inline static std::mutex _chat_mutex;
        
        inline static std::string group_path_prefix = "./client-data/groups/";
        inline static std::string chat_path_prefix = "./client-data/chats/";

        std::shared_ptr<std::queue<ChatInfo>> _chats;

        enum Type {
                GROUP,
                CHAT
        };

public:
        FileManager(std::shared_ptr<std::queue<ChatInfo>> chats)
                :
                _chats(chats)

        {
                fs::create_directories(group_path_prefix);
                fs::create_directories(chat_path_prefix);
        }

        static inline fs::path make_path(std::string hash, Type type) {
                std::string path_str;

                switch(type) {
                        case Type::GROUP: path_str = group_path_prefix; break;
                        case Type::CHAT: path_str = chat_path_prefix; break;
                }

                path_str += hash + ".txt";

                fs::path file_path = path_str;

                return file_path;
        } 

        template<class... Segs>
        void add_group(Segs... segments) {
                assert(sizeof...(segments) >= 2);

                if(sizeof...(segments) == 2) {
                        std::vector<std::string> segs = {segments...};
                        
                        std::unique_lock<std::mutex> g_lock(_group_mutex);
                        std::unique_lock<std::mutex> c_lock(_chat_mutex);
                                
                        fs::path group_path = make_path(segs[0], Type::GROUP);
                        fs::path chat_path = make_path(segs[0], Type::CHAT);

                        std::fstream group_file(group_path, std::ios::in | std::ios::out | std::ios::trunc);
                        std::ofstream chat_file(chat_path);

                        group_file << segs[1];

                        std::string name;
                        group_file.seekg(0);
                        group_file >> name;

                        Log log{};

                        log("FileManager::add_group chat_path: ", chat_path);
                        log("FileManager::add_group hash: ", segs[0]);
                        log("FileManager::add_group name: ", name);

                        _chats->push(ChatInfo(chat_path, segs[0], name));

                        return;
                }

                std::vector<std::string> segs = {segments...};

                std::unique_lock<std::mutex> g_lock(_group_mutex);
                std::unique_lock<std::mutex> c_lock(_chat_mutex);

                fs::path group_path = make_path(segs[0], Type::GROUP);
                fs::path chat_path = make_path(segs[0], Type::CHAT);

                std::fstream group_file(group_path, std::ios::in | std::ios::out | std::ios::trunc);
                std::ofstream chat_file(chat_path, std::ios::trunc);

                group_file << segs[1];
                chat_file << segs[2];

                std::string name;
                group_file.seekg(0);
                group_file >> name;

                _chats->push(ChatInfo(chat_path, segs[0], name));
        }

        template<class... Segs>
        static void append_message(Segs... segments) {
                assert(sizeof...(segments) == 2);
                std::string segs[2] = {segments...};

                std::unique_lock<std::mutex> c_lock(_chat_mutex);

                fs::path chat_path = make_path(segs[0], Type::CHAT);

                std::ofstream chat_file(chat_path, std::ios::app);

                chat_file << segs[1];
        }

};
