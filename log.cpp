#include "log.h"

#include <syslog.h>
#include <unistd.h>
#include <stdarg.h>

#include <string>

Logger::_endl Logger::endl;

Logger::Logger(){
    setlogmask (LOG_UPTO (LOG_NOTICE));
    openlog ("c-daemon", LOG_CONS | LOG_PID, LOG_LOCAL1);
    syslog (LOG_NOTICE, "daemon is started with pid: %d", getpid ());
}

void Logger::write(const char *buf, ssize_t size){
    syslog(LOG_INFO, "%s", std::string(buf, size).c_str());
}

void Logger::format(const char *format, ...){
    va_list vl;
    va_start(vl, format);
    vsyslog(LOG_NOTICE, format, vl);
    va_end(vl);
}

Logger &Logger::instance(){
    static Logger logger;
    return logger;
}
