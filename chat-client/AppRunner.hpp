#pragma once

#include <algorithm>
#include <cctype>
#include <memory.h>
#include <memory>

#include "FileManager.hpp"
#include "Reciever.hpp"
#include "TransportLayer.hpp"

#include "cpptui.hpp" //external header file from https://github.com/jonoton/cpp-tui.git

using namespace cpptui;

class AppRunner{
        std::shared_ptr<bool> LoggedIn =                std::make_shared<bool>(false);
        std::shared_ptr<ScrollableVertical>             side_bar = std::make_shared<ScrollableVertical>();
        std::shared_ptr<Label>                          header_label;
        TransportLayer                                  transport_layer;
        std::shared_ptr<ChatLogger> _chat_logger =      std::make_shared<ChatLogger>(nullptr);
        std::shared_ptr<std::string> _current_hash =    std::make_shared<std::string>("-1");
        std::shared_ptr<FileManager> file_manager =     std::make_shared<FileManager>(_current_hash, side_bar, _chat_logger, header_label);

        int _client_fd;

public:
        AppRunner(int client_fd)
                : _client_fd(client_fd)
        {
                transport_layer = TransportLayer(_client_fd, LoggedIn, file_manager);
                
                auto pd = std::make_shared<VerticalSpacer>(1);
                side_bar->add(pd);
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
                                        case Protocol::USER_NAME_TAKEN: message = "User name is taken";

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
                

                //sidebar
                auto sidebar_box = std::make_shared<Border>(BorderStyle::Rounded, Color(0, 120, 0));
                sidebar_box->fixed_width = 26;
                sidebar_box->set_title("Channels");

                side_bar->fixed_width = 24;

                //add functionality to add new group
                auto modal = std::make_shared<Dialog>(&main_app, BorderStyle::Rounded, Color(0, 100, 0));
                modal->add(std::make_shared<Label>("add group"));
                auto close_btn = std::make_shared<Button>("X close", [main_app, modal] mutable {
                        main_app.close_dialog(modal);
                });

                auto create_grp_button = std::make_shared<Button>("+ Add New", [this, main_app, modal] mutable {
                        main_app.open_dialog(modal);
                });

                sidebar_box->add(side_bar);

                //chat log
                std::shared_ptr<ScrollableVertical> chat_log = std::make_shared<ScrollableVertical>();
                chat_log->clear_children();

                *_chat_logger = ChatLogger(chat_log);

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
                                if(*_current_hash == "-1") {
                                        notify("Select a group to send to!", Notification::Type::Info);
                                        return;
                                }
                                transport_layer.send_message(*_current_hash, message);
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

                main_app.register_exit_key('q');

                //create thread for receiver
                std::thread receiver_thread{Reciever{_client_fd, file_manager}};
                main_app.run(root);

                if(receiver_thread.joinable()) receiver_thread.join();
        }

        void run() {
                login_page();
                main_page();
        }
};
