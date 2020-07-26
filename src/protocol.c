#include "protocol.h"

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

static const char *unknown_severity = "Unknown";

static const char *severity_strings[] =
{
        "Emergency",
        "Alert",
        "Critical",
        "Error",
        "Warning",
        "Notice",
        "Info",
        "Debug"
};
static const size_t SEVERITIES_COUNT = sizeof(severity_strings) / sizeof(severity_strings[0]);
static const char NILVALUE = '-';
static const char SYSLOG_VERSION = '1';
static const char FACILITY = 1; /* user-level messages */

const char * severity_to_str(const enum severity s)
{
    switch(s)
    {
        case Emergency: return severity_strings[0];
        case Alert: return severity_strings[1];
        case Critical: return severity_strings[2];
        case Error: return severity_strings[3];
        case Warning: return severity_strings[4];
        case Notice: return severity_strings[5];
        case Info: return severity_strings[6];
        case Debug: return severity_strings[7];
        default: return unknown_severity;
    }
}

int str_to_severity(const char * const str, enum severity *s)
{
    size_t i;
    for (i = 0; i < SEVERITIES_COUNT; ++i)
    {
        if (!strcmp(str, severity_strings[i]))
        {
            *s = i;
            return 0;
        }
    }
    return -1;
}

void print_severities()
{
    size_t i;
    for (i = 0; i < SEVERITIES_COUNT; ++i)
    {
        printf("%s ", severity_strings[i]);
    }
}

#define MAX_HOSTNAME_LENGTH 255

int generate_syslog_message(const struct syslog_ctl *const ctl, char buffer[SYSLOG_MESSAGE_SIZE])
{
    char fmt[64];
    char hostname[MAX_HOSTNAME_LENGTH]; /* Up to 253 octets for FQDN */
    struct timeval tv;
    struct tm *t;
    int pri_val = (FACILITY * 8) + ctl->severity;
    
    fmt[0] = 0;
    
    gettimeofday(&tv, NULL);
    if ((t = localtime(&tv.tv_sec)) != NULL)
    {
        const size_t i = strftime(fmt, sizeof(fmt), "%Y-%m-%dT%H:%M:%S.%06u%z ", t);
        fmt[i - 1] = fmt[i - 2];
		fmt[i - 2] = fmt[i - 3];
		fmt[i - 3] = ':';
    }

    /* 
     * RFC5424 states that the HOSTNAME field should contain, in the order of preference:
     * FQDN, Static IP address, hostname...
     * Keep the code simple and use just hostname here.
     */
    if (gethostname(hostname, MAX_HOSTNAME_LENGTH) == -1)
    {
        fprintf(stderr, "Cannot read hostname, error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    sprintf(buffer, "<%d>%c %s %s %s %lu %c %c %s",
            pri_val,
            SYSLOG_VERSION,
            fmt,
            hostname,
            ctl->program_name,
            ctl->pid,
            NILVALUE,
            NILVALUE,
            ctl->msg);

    printf("%s\n", buffer);
}

