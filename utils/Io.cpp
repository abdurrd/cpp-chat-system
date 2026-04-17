#include <cerrno>
#include <unistd.h>
#include <sys/socket.h>

#include "Io.hpp"

#define INTR_OCCURED errno == EINTR     // when an interupt occurs, erron is set to EINTR i.e ErrorINTRupt, 
                                        // we can try again as interupt doesn't mean there is anything wrong, 
                                        // just means attempt to read/write was made during interupt and we can 
                                        // just try again

ssize_t Io::reader(int sock, char *buffer, size_t nbyte) {
        size_t offset = 0;
        while(offset < nbyte) {
                ssize_t  chunk_size = read(sock, buffer + offset, nbyte - offset);
                if(chunk_size == 0) return offset;
                if(chunk_size < 0) {
                        if(INTR_OCCURED) continue;
                        return -1;
                }
                offset += chunk_size;
        }

        return offset;
}

        
ssize_t Io::writer(int sock, char *buffer, size_t nbyte) {
        size_t offset = 0;
        while(offset < nbyte) {
                ssize_t chunk_size = send(sock, buffer + offset, nbyte - offset, 0);
                if(chunk_size < 0) {
                        if(INTR_OCCURED) continue; //if(interupt) continue;
                        return -1;
                }
                offset += chunk_size;

        }

        return offset;
}





