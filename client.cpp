#include "client.h"

#include <iostream>
#include <sstream>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#include "log.h"

class Client::impl{
public:
    impl(int fd, struct sockaddr addr)
        : m_messageLeft(0)
    {
        /* Log the address */
        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
        if(-1 != getnameinfo(&addr, sizeof(addr), hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV)){
            LOGGER << "Accepted connection on fd" << fd << "host:" << hbuf << ", port:" << sbuf << Logger::endl;
        }
    }
    void retrieveRawBuffer(char *buf, ssize_t size);
private:
    void ProcessMessage();
    void ReceiveProgram();
    void CompileProgram();
    void RunProgram();

    std::stringstream m_buf;
    ssize_t m_messageLeft;
};

void Client::impl::retrieveRawBuffer(char *buf, ssize_t size){
    (void) buf, (void) size;
//    while(size){
//        if(m_messageLeft == 0){
//
//        }else if(m_messageLeft <= size){
//            m_buf.write(buf, m_messageLeft);
//            m_messageLeft = 0;
//            size -= m_messageLeft;
//            buf += m_messageLeft;
//            ProcessMessage();
//        }else if(m_messageLeft > size){
//            m_buf.write(buf, size);
//            m_messageLeft -= size;
//            size = 0;
//        }
//    }
}

void Client::impl::ProcessMessage(){
    /* switch message type and call corresponding handler */
}
void Client::impl::ReceiveProgram(){
    /* save source to a temporary file */
}
void Client::impl::CompileProgram(){
    /* invoke compiler */
}
void Client::impl::RunProgram(){
    /* setup a new process and call to the new code when incoming data */
}

Client::Client(int fd, struct sockaddr addr) : d(new impl(fd, addr)){ }

void Client::retrieveRawBuffer(char *buf, ssize_t size){
    LOGGER.write(buf, size);
    d->retrieveRawBuffer(buf, size);
}
