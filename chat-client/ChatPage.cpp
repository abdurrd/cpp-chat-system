#include "ChatPage.hpp"

ChatPage::ChatPage(std::shared_ptr<TransportLayer> transport_layer, int client, std::shared_ptr<std::queue<ChatInfo>> chat_queue, std::shared_ptr<std::string> user, std::shared_ptr<bool> loggedin, std::shared_ptr<FileManager> file_manager) 
        :
        Page(transport_layer),
        _client_fd(client),
        _loaded_chats(chat_queue),
        _username(user),
        _loggedIn(loggedin),
        _file_manager(file_manager)
{}

void ChatPage::construct() {

        auto notif = std::make_shared<Notification>();
        Page::set_notification(notif);

        auto notify = [notif] (std::string message, Notification::Type type){
                notif->position = Notification::Position::TopRight;
                notif->show(message, type, 3000);
        };
        
        auto header = std::make_shared<Horizontal>();
        header->fixed_height = 1;
        std::shared_ptr<Label> header_label = std::make_shared<Label>("-- No chat opened --");

        header->add(std::make_shared<HorizontalSpacer>(1));
        header->add(header_label);
        header->add(std::make_shared<HorizontalSpacer>(1));

        //sidebar
        auto sidebar_box = std::make_shared<Border>(BorderStyle::Rounded, Color(0, 120, 0));
        sidebar_box->fixed_width = 26;
        sidebar_box->set_title("Channels");

        auto side_bar = std::make_shared<ScrollableVertical>();
        auto pd = std::make_shared<VerticalSpacer>(1);
        side_bar->add(pd);
        side_bar->fixed_width = 24;

        //add functionality to add new group
        auto dlg = std::make_shared<Dialog>(Page::getApp(), BorderStyle::Rounded);
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

                _transport_layer->create_group(grp_name, members.size(), members);

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

        auto send_btn = std::make_shared<Button>("Send", [this, message_input, chat_log, notify] {
                        std::string message = message_input->get_value();
                        if(message.length() == 0) return;
                        if(focused_chat.getHash() == "-1") {
                                notify("Select a group to send to!", Notification::Type::Info);
                                return;
                        }
                        _transport_layer->send_message(focused_chat.getHash(), message);

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
        chat_panel->add(header);
        chat_panel->add(log_box);
        chat_panel->add(input_row);

        panel_box->add(chat_panel);

        //root
        layout = std::make_shared<Horizontal>();
        layout->add(sidebar_box);
        layout->add(panel_box);

        auto cur_chat_file = std::make_shared<std::ifstream>();
        auto read_content = std::make_shared<bool>(false);

        Page::add_callback(100, [this, cur_chat_file, read_content, chat_log, side_bar, header]{
                while(!_loaded_chats->empty()) {
                        ChatInfo cht = _loaded_chats->front();
                        std::shared_ptr<Button> btn; 
                        auto btn_holder = std::make_shared<std::shared_ptr<Button>>(nullptr); 
                        btn = std::make_shared<Button>(cht.getName(), [this, btn_holder, cht, chat_log, header, cur_chat_file] {
                                focused_chat = cht;

                                chat_log->clear_children();
                                header->clear_children();
                                
                                std::shared_ptr<Label> header_label = std::make_shared<Label>(cht.getName());
                                header->add(std::make_shared<HorizontalSpacer>(1));
                                header->add(header_label);

                                if(cht.isAdmin()) {
                                        auto delete_button = std::make_shared<Button>("Delete", [this, btn_holder, header, cht, chat_log]{
                                                std::shared_ptr<Label> header_label = std::make_shared<Label>("-- No chat opened --");
                                                header->clear_children();
                                                header->add(std::make_shared<HorizontalSpacer>(1));
                                                header->add(header_label);
                                                header->add(std::make_shared<HorizontalSpacer>(1));

                                                chat_log->clear_children();

                                                (*btn_holder)->visible = false;

                                                //transport_layer.delete_group(cht.getHash());
                                        });

                                        delete_button->bg_color = {150, 20, 0};
                                        delete_button->hover_color = {120, 20, 0};
                                        delete_button->focus_color = {180, 10, 0};

                                        delete_button->fixed_height = 1;
                                        delete_button->fixed_width = 10;

                                        header->add(delete_button);
                                }
                                
                                header->add(std::make_shared<HorizontalSpacer>(1));


                                cur_chat_file->close();
                                cur_chat_file->clear();
                                cur_chat_file->open(cht.getPath());
                                cur_chat_file->seekg(0);
                        });
                        btn->fixed_height = 3;
                        *btn_holder = btn;
                        side_bar->add(btn);
                        _loaded_chats->pop();
                }

                if(*read_content) {
                        chat_log->scroll_offset = std::max(0, (chat_log->content_height - chat_log->height));
                        *read_content = false;
                }

                if(!cur_chat_file->is_open()) return;
                std::string content;
                while(std::getline(*cur_chat_file, content)) {
                        *read_content = true;
                        std::istringstream parsed(content);
                        std::string sender;
                        std::string message;

                        std::getline(parsed, sender, ' ');
                        std::getline(parsed, message, '\n');

                        auto border = std::make_shared<Border>(BorderStyle::Rounded, Color{0, 120, 0});
                        border->fixed_height = 4;
                        border->responsive_width = true;
                        border->bg_color = (*_username == sender) ? Color{0, 120, 0} : Color{0, 0, 0};
                        border->fg_color = (*_username == sender) ? Color{0, 120, 0} : Color{0, 0, 0};

                        auto bubble = std::make_shared<Vertical>();
                        bubble->fixed_height = 4;
                        bubble->add(std::make_shared<Label>(sender));
                        bubble->add(std::make_shared<Label>(message));

                        border->add(bubble);
                        
                        chat_log->add(border);
                }

                if(cur_chat_file->eof()) cur_chat_file->clear();                
        });

        Page::set_exit_key('q');
}
