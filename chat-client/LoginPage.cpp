#include "LoginPage.hpp"

LoginPage::LoginPage(std::shared_ptr<TransportLayer> transport_layer) : Page(transport_layer){}

void LoginPage::construct() {
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
        Page::set_notification(notif);

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

                int result = _transport_layer->login_user(u_username, u_password);

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

                Page::quit();
        });

        auto signup_btn = std::make_shared<Button>("Signup", [&] {
                u_username = username_input->get_value();
                u_password = pass_input->get_value();
                
                bool valid = validation_check(u_username, u_password);

                if(!valid) return;

                int result = _transport_layer->create_user(u_username, u_password);

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
                Page::quit();
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

        layout = std::make_shared<Align>(Align::H::Center, Align::V::Center);

        layout->add(login_border);
        layout->add(notif);
}
