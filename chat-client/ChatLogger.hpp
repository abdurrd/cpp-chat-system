#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <thread>
#include "cpptui.hpp"

class ChatLogger {
        bool _run;
        std::shared_ptr<cpptui::ScrollableVertical> _chat_log;
        std::thread log_thread;
public:

        ChatLogger(std::shared_ptr<cpptui::ScrollableVertical> chat_log)
                :
                _run(false),
                _chat_log(chat_log)
        {}

        static void log(std::filesystem::path chat_path, bool *run, std::shared_ptr<cpptui::ScrollableVertical> chat_log) {
                std::ifstream chat_file(chat_path);
                if(chat_file.is_open() == false ){
                        //notify
                        return;
                }

                chat_file.seekg(0);

                std::string msg;
                while(*run) {
                        while(*run && std::getline(chat_file, msg)){
                                auto message = std::make_shared<cpptui::Label>(msg);
                                chat_log->add(message);
                        }
                        if(chat_file.eof()){
                                chat_file.clear();
                        }
                        if(chat_file.bad()) {
                                //notify
                                break;
                        }

                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

        }

        void start_log(std::filesystem::path path) {
                _run = true;
                log_thread = std::thread(log, path, &_run, _chat_log);
        }

        void end_log() {
                _run = false;
                if(log_thread.joinable()) log_thread.join();
        }
};
