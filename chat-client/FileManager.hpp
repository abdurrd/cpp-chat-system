#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <mutex>
#include <ranges>

#include "ChatLogger.hpp"
#include "cpptui.hpp"

namespace fs = std::filesystem;

class FileManager {
        inline static std::mutex _group_mutex;
        inline static std::mutex _chat_mutex;
        
        inline static std::string group_path_prefix = "./client-data/groups/";
        inline static std::string chat_path_prefix = "./client-data/chats/";

        std::shared_ptr<std::string> _current_hash;
        std::shared_ptr<ChatLogger> _chat_logger;
        std::shared_ptr<cpptui::ScrollableVertical> _sidebar_ptr;
        std::shared_ptr<cpptui::Label> _header;

        enum Type {
                GROUP,
                CHAT
        };

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
        
public:
        FileManager(std::shared_ptr<std::string> current_hash, std::shared_ptr<cpptui::ScrollableVertical> sidebar, std::shared_ptr<ChatLogger> chatlog, std::shared_ptr<cpptui::Label> header)
                :
                _current_hash(current_hash),
                _sidebar_ptr(sidebar),
                _chat_logger(chatlog)
        {
                fs::create_directories(group_path_prefix);
                fs::create_directories(chat_path_prefix);
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

                        std::ofstream group_file(group_path, std::ios::trunc);
                        std::ofstream chat_file(chat_path);

                        group_file << segs[1];

                        std::vector<std::string> parsed = segs[1] | std::views::split('\n') | std::ranges::to<std::vector<std::string>>();
                        std::string grp = parsed[0];
                        std::string hash = segs[0];
                        auto btn = std::make_shared<cpptui::Button> (grp, [this, grp, chat_path, hash](){
                                _sidebar_ptr->clear_children();
                                _header = std::make_shared<cpptui::Label>(grp);
                                _chat_logger->end_log();
                                _chat_logger->start_log(chat_path);
                                *_current_hash = hash;
                        });
                        _sidebar_ptr->add(btn);
                        
                        return;
                }

                std::vector<std::string> segs = {segments...};

                std::unique_lock<std::mutex> g_lock(_group_mutex);
                std::unique_lock<std::mutex> c_lock(_chat_mutex);

                fs::path group_path = make_path(segs[0], Type::GROUP);
                fs::path chat_path = make_path(segs[0], Type::CHAT);

                std::ofstream group_file(group_path, std::ios::trunc);
                std::ofstream chat_file(chat_path, std::ios::trunc);

                group_file << segs[1];
                chat_file << segs[2];

                std::vector<std::string> parsed = segs[1] | std::views::split('\n') | std::ranges::to<std::vector<std::string>>();
                std::string grp = parsed[0];
                auto btn = std::make_shared<cpptui::Button> (grp, [this, grp, chat_path](){
                        _sidebar_ptr->clear_children();
                        _header = std::make_shared<cpptui::Label>(grp);
                        _chat_logger->end_log();
                        _chat_logger->start_log(chat_path);
                });
                _sidebar_ptr->add(btn);
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
