#include "compiler.h"

#include <unistd.h>
#include <stdexcept>
#include <sys/wait.h>

/* TODO: create common interface for different compiler */
void compile(const char *sourceName, const char *binaryName){
    const char* compilerBin = "gcc";
    const char * argv[] = { compilerBin, "-o", binaryName, sourceName };
    const char * env[] = { NULL };

    int pid;
    pid = fork();
    if(0 == pid){ /* Child */
        if(-1 == execve(compilerBin, const_cast<char**>(argv), const_cast<char**>(env))){
            throw std::runtime_error("execve");
        }
    }else if(pid > 0){
        int status;
        waitpid(pid, &status, 0);
        /* TODO: handle compiler errror */
    }else{
        throw std::runtime_error("fork");
    }
}
