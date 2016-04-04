#include "server.h"

#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <cstdio>
#include <cstring>
#include <map>
#include <memory>

#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "client.h"

namespace Server {

namespace{

struct Config{
    const char *ip;
    uint16_t port;
};
static Config _config = { "127.0.0.1", 15873 };
static const int MAXEVENTS = 64;
static const ssize_t BUF_SIZE = 512;

static std::map<int, std::shared_ptr<Client>> clientFdMap;

/* System handles */
static int socketfd = -1;
static int epollfd = -1;
struct epoll_event event;
struct epoll_event *events;

static void nonBlocking(int fd);
static void createAndBind();
static void processEvent(struct epoll_event &event);
static void handleConnection(struct epoll_event &event);
static void handleData(struct epoll_event &event);
static void listen();

void nonBlocking(int fd){
    int flags = fcntl(fd, F_GETFL, 0);
    if(-1 == flags){
        throw std::runtime_error("fcntl");
    }
    flags |= O_NONBLOCK;
    if(-1 == fcntl(fd, F_SETFL, flags)){
        throw std::runtime_error("fcntl");
    }
}

void createAndBind(){
    /* Create socket fd */
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd < 0){
        throw std::runtime_error("socket");
    }

    /* Server configuration */
    /* XXX: not specified bind ip yet, bind to 0.0.0.0(all) ipv4 currently */
    struct sockaddr_in bind_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(_config.port),
        .sin_addr = {.s_addr = htonl(INADDR_ANY)},
    };
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (struct sockaddr *)&bind_addr, sizeof(bind_addr));

    /* Bind to any address:PORT */
    if(bind(socketfd, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0){
        throw std::runtime_error("bind");
    }

    nonBlocking(socketfd);
}

void processEvent(struct epoll_event &event){
    if((event.events & EPOLLERR) ||
       (event.events & EPOLLHUP) ||
       (!(event.events & EPOLLIN))){
        throw std::runtime_error("epoll event error");
    }else if(socketfd == event.data.fd){
        handleConnection(event);
    }else{
        handleData(event);
    }
}

void handleConnection(struct epoll_event &event){
    while(1){

        struct sockaddr in_addr;
        socklen_t in_len = sizeof(in_addr);
        int infd = accept(socketfd, &in_addr, &in_len);

        /* Check if more connection to be processed */
        if(-1 == infd){
            if(EAGAIN == errno || EWOULDBLOCK == errno){
                /* All incoming connections is processed */
               break;
           }else{
               /* TODO: check the proper way to deal with this error */
               throw std::runtime_error("accept");
           }
        }

        nonBlocking(infd);

        /* Add to epoll list */
        event.data.fd = infd;
        event.events = EPOLLIN | EPOLLET;
        if(-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, infd, &event)){
            throw std::runtime_error("epoll_ctl");
        }

        auto pClient = std::make_shared<Client>(infd, in_addr);
        clientFdMap[infd] = pClient;
    }
}

void handleData(struct epoll_event &event){
    ssize_t size;
    char inbuf[BUF_SIZE];
    while(1){
        if(-1 == (size = read(event.data.fd, inbuf, BUF_SIZE))){
            if(errno != EAGAIN){
                throw std::runtime_error("read");
            }else{
                break;
            }
        }
        if(size == 0) break;
        clientFdMap[event.data.fd]->retrieveRawBuffer(inbuf, size);
    }
}

/* Enter an event loop with epoll */
void listen(){
    if(-1 == ::listen(socketfd, SOMAXCONN)){
        throw std::runtime_error("listen");
    }
    if(-1 == (epollfd = epoll_create1(EPOLL_CLOEXEC))){
        throw std::runtime_error("epoll");
    }
    event.data.fd = socketfd;
    event.events = EPOLLIN | EPOLLET;
    if(-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &event)){
        throw std::runtime_error("epoll_ctl");
    }

    std::unique_ptr<epoll_event[]> events(new epoll_event[MAXEVENTS]);

    while(1){
        int nEvent = epoll_wait(epollfd, events.get(), MAXEVENTS, -1);
        for(int i = 0;i < nEvent;++i){
            try{
                processEvent(events[i]);
            }catch(std::runtime_error &err){
                std::cerr << "Error during epoll event loop" << std::endl;
                perror(err.what());
            }
        }
    }

}

} // namespace anonymous

void config(const char *ip, uint16_t port, int buf_size){
    _config.ip = ip;
    _config.port = port;
}

void start(){
    try{
        createAndBind();
        std::cout<<"socket created" << std::endl;
        listen();
    } catch(const std::runtime_error &e){
        perror(e.what());
    }
}

void stop(){

}

} // namespace Server
