#pragma once
#include "Page.hpp"
#include "../Protocol.hpp"

class LoginPage : public Page {
public:
        LoginPage(std::shared_ptr<TransportLayer> transport_layer);

        void construct() override;
};
