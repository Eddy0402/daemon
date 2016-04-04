#ifndef CLIENT_H__
#define CLIENT_H__

#include <unistd.h>
#include <memory>

class Client{
public:
    explicit Client(int fd, struct sockaddr addr);
    void retrieveRawBuffer(char *buf, ssize_t size);
private:
    class impl; std::unique_ptr<impl> d;
};

#endif
