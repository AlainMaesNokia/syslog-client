#include "options.h"

#include <stdio.h>
#include <getopt.h>
#include <limits.h>
#include <unistd.h>

static const unsigned short DEFAULT_PORT = 514;
static const enum severity DEFAULT_SEVERITY = Info;

static void usage(const char * const program_name, const int status)
{
    if (status != EXIT_SUCCESS)
    {
        fprintf(stderr, "Try '%s --help' for more information.\n", program_name);
    }
    else
    {
        FILE *out = stdout;
        printf("Usage: %s [OPTION]...\n", program_name);
        /* fputs does not parse the string to find formatting characters */
        fputs("Send a message to a remote syslog server\n\n", out);
        fputs("Mandatory arguments to long options are mandatory for short options too.\n", out);
        fputs("\t-r, --remote=IP_ADDRESS    remote address\n", out);
        printf("\t-p, --port=PORT            communication port, optional, default is %d\n", DEFAULT_PORT);
        printf("\t-s, --severity=SEVERITY    severity, optional, default is %s, severity options are:\n", severity_to_str(DEFAULT_SEVERITY));
        fputs("                                    [ ", out);
        print_severities();
        fputs("]\n", out);
        fputs("\t-h, --help\tshow this help\n", out);
    }
    exit(status);
}

static void set_default_options(struct syslog_ctl *ctl)
{
    ctl->addr = NULL;
    ctl->port = DEFAULT_PORT;
    ctl->severity = Info;
    ctl->msg = NULL;
}

int parse_options(const int argc, char **argv, struct syslog_ctl *ctl)
{
    static struct option long_options[] =
    {
        {"remote", required_argument, NULL, 'r'},
        {"port", optional_argument, NULL, 'p'},
        {"severity", optional_argument, NULL, 's'},
        {"message", optional_argument, NULL, 'm'},
        {"msg-file", optional_argument, NULL, 'f'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    int opt = 0, oi = -1;

    set_default_options(ctl);

    ctl->program_name = argv[0];

    while ((opt = getopt_long(argc, argv, "hr:p:s:m:f:", long_options, &oi)) != -1)
    {
        switch(opt)
        {
            case 'r' : ctl->addr = optarg; break;
            case 'p' :
            {
                int port;
                if ((port = atoi(optarg)) > USHRT_MAX)
                {
                    fprintf(stderr, "'%d' is invalid port!\n", port);
                    usage(argv[0], EXIT_FAILURE);
                }
                ctl->port = port;
                break;
            }
            case 's' : 
            {
                if (str_to_severity(optarg, &ctl->severity))
                {
                    fprintf(stderr, "'%s' is invalid severity!\n", optarg);
                    usage(argv[0], EXIT_FAILURE);
                }
                break;
            }
            case 'm' :
            {
                ctl->msg = optarg;
                break;
            }
            case 'f':
            {
                printf("Not implemented\n");
                break;
            }
            case 'h' : usage(argv[0], EXIT_SUCCESS); return 0;
            case '?' : break;
            default : printf("getopt_long returned character code 0%o\n", opt);
        }
    }

    if (!ctl->addr)
    {
        fputs("The 'remote' argument is mandatory!\n", stderr);
        usage(argv[0], EXIT_FAILURE);
        return -1;
    }

    ctl->pid = getpid();
/* Ignore non-option arguments. */
/*
    if (optind < argc) {
        printf("non-option argv elements: ");
        while (optind < argc)
        {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
    }
*/
    return 0;
}
