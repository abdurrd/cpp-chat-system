#include "Page.hpp"

Page::Page(std::shared_ptr<TransportLayer> transport_layer) 
        :
        _transport_layer(transport_layer)
{
        Theme::set_theme(Theme::TokyoNight());
}


App* Page::getApp() {
        return &app;
}

void Page::add_callback(int interval, std::function<void(void)> callback){
        app.add_timer(interval, callback);
}

void Page::render() {
        construct();
        app.run(layout);
}

void Page::set_notification(std::shared_ptr<Notification> notfication) {
        app.set_notification(notfication);
}

void Page::set_exit_key(char key) {
        app.register_exit_key(key);
}

void Page::quit() {
        app.quit();
}
