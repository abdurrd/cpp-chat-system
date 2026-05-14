#pragma once

#include <fstream>

class Log {
        std::ofstream _log{"./client-data/logs.txt", std::ios::app};
public:
        void operator()(std::string prefix, std::string mesg) {
                _log << prefix << ": " << mesg << std::endl;
        }
};
