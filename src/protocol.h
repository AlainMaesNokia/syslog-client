#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <sys/types.h>

#define SYSLOG_MESSAGE_SIZE 1024

enum severity
{
    Emergency = 0,  /* system is unusable */
    Alert = 1,      /* action must be taken immediately */
    Critical = 2,   /* critical conditions */
    Error = 3,      /* error conditions */
    Warning = 4,    /* warning conditions */
    Notice = 5,     /* normal but significant condition */
    Info = 6,       /* informational messages */
    Debug = 7       /* debug-level messages */
};

const char *severity_to_str(const enum severity s);
int str_to_severity(const char * const str, enum severity *s);
void print_severities();

struct syslog_ctl
{
    const char *program_name;
    const char *addr;
    unsigned short port;
    enum severity severity;
    pid_t pid;
    const char *msg;
};

int generate_syslog_message(const struct syslog_ctl *const ctl, char buffer[SYSLOG_MESSAGE_SIZE]);

#endif /* PROTOCOL_H */
