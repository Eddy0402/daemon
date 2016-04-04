#include <unistd.h>
#include <cstdlib>
#include <thread>

#include "server.h"

pid_t sid;

void daemonize(){
    pid_t pid = fork();
    if (pid < 0) {
        /* syslog */
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        /* daemon process created */
        exit(EXIT_SUCCESS);
    }
    sid = setsid();
    if(sid < 0){
        /* syslog */
        exit(EXIT_FAILURE);
    }
    /* set mask */
}

int main(int argc, char **argv){
    daemonize();

    std::thread serverThread([](){
        Server::start();
    });

    serverThread.join();
}
