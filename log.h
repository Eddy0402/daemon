#ifndef LOG_H__
#define LOG_H__

#include <sstream>
#include <syslog.h>
#include <stdint.h>

class Logger
{
private:
    Logger();

public:

    template <class T>
    Logger &operator<<(const T&rhs)
    {
        buf << rhs;
        return *this;
    }

    static struct _endl{} endl;
    Logger &operator<<(const _endl &dummy)
    {
        (void) dummy;
        syslog(LOG_INFO, "%s", buf.str().c_str());
        buf.str(std::string()); /* Clear the buffer */
        return *this;
    }

    void write(const char *buf, ssize_t size);
    void format(const char *format, ...) __attribute__ ((format (printf, 2, 3)));

    std::stringstream buf;

    static Logger &instance();
};

#define LOGGER Logger::instance()

#endif
