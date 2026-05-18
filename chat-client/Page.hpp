#pragma once
#include "TransportLayer.hpp"
#include "cpptui.hpp"
using namespace cpptui;

class Page {
        App app;
protected:
        std::shared_ptr<TransportLayer> _transport_layer;
        std::shared_ptr<Container> layout;
public:
        Page(std::shared_ptr<TransportLayer> transport_layer);
        virtual ~Page() = default;

        virtual void construct() = 0;
        void render();

        App* getApp();

        void add_callback(int interval, std::function<void(void)> callback);
        void set_notification(std::shared_ptr<Notification> notfication);
        void set_exit_key(char key);

        void quit();
};
