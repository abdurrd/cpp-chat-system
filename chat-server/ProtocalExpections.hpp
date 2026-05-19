#include <iostream>
#include <exception>

#include "../Protocol.hpp"

class ProtocalExpection : public std::exception {
        Protocol _proto;
        mutable std::string _err_msg;
public:
        ProtocalExpection(Protocol proto);
        const char * what() const noexcept override;

};
