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
        AppRunner(int client_fd)
                : _client_fd(client_fd)
        {
                transport_layer = TransportLayer(_client_fd, LoggedIn, file_manager);
        }

        void login_page() {
                Theme::set_theme(Theme::TokyoNight());

                App app;
                
                std::string u_username;
                std::string u_password;

                auto login_box = std::make_shared<Border>(BorderStyle::Rounded, Color(0, 120, 0));
                login_box->fixed_height = 3;

                auto username_input = std::make_shared<Input>();
                username_input->placeholder = "Username...";

                login_box->add(username_input);

                auto pass_box = std::make_shared<Border>(BorderStyle::Rounded, Color(0, 120, 0));
                pass_box->fixed_height = 3;

                auto pass_input = std::make_shared<Input>();
                pass_input->placeholder = "Password...";
                pass_input->is_password = true;

                pass_box->add(pass_input);

                auto notif = std::make_shared<Notification>();
                app.set_notification(notif);

                auto notify = [notif] (std::string message, Notification::Type type){
                        notif->position = Notification::Position::TopRight;
                        notif->show(message, type, 3000);
                };

                auto validation_check = [notify](std::string username, std::string password) {
                        if(username.length() <= 4 || username.length() > 16) {
                                notify("username should be 5-16 characters", Notification::Type::Warning);
                                return false;
                        }

                        if(password.length() <= 6 || password.length() > 16) {
                                notify("password should be 6 characters", Notification::Type::Warning);
                                return false;
                        }

                        bool invalid_username = std::ranges::any_of(username, [](char &c) {
                                return !std::isalpha((unsigned char)c);
                        });

                        if(invalid_username) {
                                notify("username should only have aphabets", Notification::Type::Warning);
                                return false;
                        }

                        bool invalid_password = std::ranges::any_of(password, [](char &c) {
                                return c == ' ';
                        });

                        if(invalid_password) {
                                notify("password should not have spaces", Notification::Type::Warning);
                                return false;
                        }

                        return true;
                };

                auto btn_sec = std::make_shared<Horizontal>();

                auto login_btn = std::make_shared<Button>("Login", [&] {
                        u_username = username_input->get_value();
                        u_password = pass_input->get_value();
                        
                        bool valid = validation_check(u_username, u_password);

                        if(!valid) return;

                        int result = transport_layer.login_user(u_username, u_password);

                        if(result != 1) {
                                std::string message = "";
                                switch(static_cast<Protocol>(result)) {
                                        case Protocol::USER_NOT_FOUND: message = "User Not Found"; break;
                                        case Protocol::WRG_PASSWORD: message = "Wrong password"; break;

                                        default: message = "Server Error";
                                } 
                                notify(message, Notification::Type::Error);
                                return;
                        }

                        notify("Login Successful", Notification::Type::Success);

                        app.quit();
                });

                auto signup_btn = std::make_shared<Button>("Signup", [&] {
                        u_username = username_input->get_value();
                        u_password = pass_input->get_value();
                        
                        bool valid = validation_check(u_username, u_password);

                        if(!valid) return;

                        int result = transport_layer.create_user(u_username, u_password);

                        if(result != 1) {
                                std::string message = "";
                                switch(static_cast<Protocol>(result)) {
                                        case Protocol::USER_NAME_TAKEN: message = "User name is taken"; break;

                                        default: message = "Server Error";
                                } 
                                notify(message, Notification::Type::Error);
                                return;
                        }

                        notify("Signup and Login Successful", Notification::Type::Success);
                        app.quit();
                });

                btn_sec->add(login_btn);
                btn_sec->add(signup_btn);

                auto login_form = std::make_shared<Vertical>();
                login_form->fixed_height = 15;

                login_form->add(std::make_shared<Label>("Welcome"));
                login_form->add(std::make_shared<VerticalSpacer>(1));

                login_form->add(login_box);
                login_form->add(std::make_shared<VerticalSpacer>(1));

                login_form->add(pass_box);
                login_form->add(std::make_shared<VerticalSpacer>(1));

                login_form->add(btn_sec);

                auto login_border = std::make_shared<Border>(BorderStyle::Rounded, Color(0, 120, 0));
                login_border->fixed_width = 40;
                login_border->fixed_height = 15;
                login_border->set_title("Login");

                login_border->add(login_form);

                auto centered = std::make_shared<Align>(Align::H::Center, Align::V::Center);

                centered->add(login_border);
                centered->add(notif);

                app.run(centered); // blocks until login_btn calls app.quit()
        }

        void main_page() {

                Theme::set_theme(Theme::TokyoNight());

                App main_app;


                auto notif = std::make_shared<Notification>();
                main_app.set_notification(notif);

                auto notify = [notif] (std::string message, Notification::Type type){
                        notif->position = Notification::Position::TopRight;
                        notif->show(message, type, 3000);
                };
                
                std::shared_ptr<Label> header_label = std::make_shared<Label>("-- No chat opened --");

                //sidebar
                auto sidebar_box = std::make_shared<Border>(BorderStyle::Rounded, Color(0, 120, 0));
                sidebar_box->fixed_width = 26;
                sidebar_box->set_title("Channels");

                auto side_bar = std::make_shared<ScrollableVertical>();
                auto pd = std::make_shared<VerticalSpacer>(1);
                side_bar->add(pd);
                side_bar->fixed_width = 24;

                //add functionality to add new group
                auto dlg = std::make_shared<Dialog>(&main_app, BorderStyle::Rounded);
                dlg->set_title(" Enter Name ");
                dlg->width = 40;
                dlg->height = 10;
                dlg->modal = true;
                dlg->shadow = true;
                dlg->bg_color = {30, 130, 50};

                auto d_v = std::make_shared<Vertical>();
                auto name_inp = std::make_shared<Input>();
                name_inp->placeholder = "Group Name...";
                name_inp->fixed_height = 1;
                name_inp->bg_color = {20, 20, 70};

                auto mem_inp = std::make_shared<Input>();
                mem_inp->placeholder = "Members...";
                mem_inp->fixed_height = 1;
                mem_inp->bg_color = {20, 20, 70};

                d_v->add(std::make_shared<Label>("Group Name:"));
                d_v->add(name_inp);
                d_v->add(std::make_shared<Label>("Member Names:"));
                d_v->add(mem_inp);
                d_v->add(std::make_shared<VerticalSpacer>());

                auto h_btns = std::make_shared<Horizontal>();
                h_btns->fixed_height = 1;

                auto btn_submit = std::make_shared<Button>("Submit", [this, dlg, name_inp, mem_inp, notify] { 
                        std::string grp_name = name_inp->get_value();
                        std::string member_str = mem_inp->get_value();

                        bool invalid_grpname = std::ranges::any_of(grp_name, [](char &c) {
                                return !std::isalpha((unsigned char)c);
                        });

                        if(invalid_grpname) {
                                notify("Group name can only have alphabets!", Notification::Type::Error);
                        }

                        auto members = member_str 
                                | std::views::split(' ') 
                                | std::ranges::to<std::vector<std::string>>();

                        transport_layer.create_group(grp_name, members.size(), members);

                        name_inp->set_value("");
                        mem_inp->set_value("");
                        dlg->close(); 
                });
                btn_submit->bg_color = Theme::current().success;
                h_btns->add(btn_submit);

                h_btns->add(std::make_shared<Label>("  "));

                auto btn_cancel = std::make_shared<Button>("Cancel", [dlg] { dlg->close(); });
                btn_cancel->bg_color = Theme::current().error;
                h_btns->add(btn_cancel);

                d_v->add(h_btns);
                dlg->add(d_v);

                auto cgb_border = std::make_shared<Border>(BorderStyle::Rounded, Color(0, 100, 0));
                auto create_grp_button = std::make_shared<Button>("+ Add New", [dlg] {
                        dlg->open(30, 4);
                });
                cgb_border->fixed_height = 3;
                cgb_border-> add(create_grp_button);
                side_bar->add(cgb_border);

                side_bar->add(pd);

                sidebar_box->add(side_bar);

                //chat log
                std::shared_ptr<ScrollableVertical> chat_log = std::make_shared<ScrollableVertical>();
                chat_log->clear_children();

                auto log_box = std::make_shared<Border>(BorderStyle::Rounded, Color(0, 150, 0));

                log_box->add(chat_log);

                //input row
                auto message_box = std::make_shared<Border>(BorderStyle::Double, Color(0, 150, 0));

                std::shared_ptr<Input> message_input = std::make_shared<Input>();
                message_input->bg_color = {26, 27, 38};
                message_input->placeholder = "Type a message...";

                message_box->add(message_input);

                auto send_box = std::make_shared<Border> (BorderStyle::Rounded, Color(0, 150, 0));
                send_box->fixed_width = 15;

                auto send_btn = std::make_shared<Button>("Send", [this, message_input, notify] {
                                std::string message = message_input->get_value();
                                if(message.length() == 0) return;
                                if(focused_chat.getHash() == "-1") {
                                        notify("Select a group to send to!", Notification::Type::Info);
                                        return;
                                }
                                transport_layer.send_message(focused_chat.getHash(), message);
                                message_input->set_value("");
                });
                send_btn->bg_color = {26, 27, 38};
                send_btn->hover_color = {26, 100, 38};
                send_btn->focus_color = {26, 27, 38};

                send_box->add(send_btn);

                auto input_row = std::make_shared<Horizontal>();
                input_row->fixed_height = 3;

                input_row->add(message_box);

                auto hpd = std::make_shared<HorizontalSpacer>(1);

                input_row->add(hpd);
                input_row->add(send_box);

                //chat panel
                auto panel_box = std::make_shared<Border>(BorderStyle::Rounded, Color(0, 100, 38));

                auto chat_panel = std::make_shared<Vertical>();
                chat_panel->add(header_label);
                chat_panel->add(log_box);
                chat_panel->add(input_row);

                panel_box->add(chat_panel);

                //root
                auto root = std::make_shared<Horizontal>();
                root->add(sidebar_box);
                root->add(panel_box);

                std::ifstream cur_chat_file;
                
                main_app.add_timer(100, [&]{
                        while(!loaded_chats->empty()) {
                                ChatInfo cht = loaded_chats->front();
                                auto btn = std::make_shared<Button>(cht.getName(), [this, cht, chat_log, header_label, &cur_chat_file] {
                                        focused_chat = cht;
                                        chat_log->clear_children();
                                        header_label->set_text(cht.getName());
                                        cur_chat_file.close();
                                        cur_chat_file.clear();
                                        cur_chat_file.open(cht.getPath());
                                        cur_chat_file.seekg(0);
                                });
                                btn->fixed_height = 3;
                                side_bar->add(btn);
                                loaded_chats->pop();
                        }

                        if(!cur_chat_file.is_open()) return;
                        std::string message;
                        while(std::getline(cur_chat_file, message)) {
                                chat_log->add(std::make_shared<Label>(message));
                        }
                        if(cur_chat_file.eof()) cur_chat_file.clear();                
                });

                main_app.register_exit_key('q');

                //create thread for receiver
                std::thread receiver_thread{Reciever{_client_fd, file_manager, _running}};
                main_app.run(root);

                *_running = false;
                if(receiver_thread.joinable()) receiver_thread.join();
        }

        void run() {
                *_running = true;
                login_page();
                main_page();
        }
};
