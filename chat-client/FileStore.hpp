#pragma once
#include <sstream>
#include "../Protocol.hpp"

template<class Derived>
class FileManager {
public:
        virtual ~FileManager() = default;

        template<class... Args>
        inline void handler(Args... args){
               dynamic_cast<Derived*>(this)->process_payload(args...); //add exception for nullptr
        }
};
 
class FileInit: public FileManager<FileInit> {
public:
        template<class... Args>
        void process_payload(Args... args) {

        }
};
