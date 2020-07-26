#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "options.h"
#include "protocol.h"

int main(int argc, char **argv)
{

    struct syslog_ctl ctl;
    parse_options(argc, argv, &ctl);
    printf("r: %s\np: %d\ns: %s\npid=%lu\nmsg='%s'\n", ctl.addr, ctl.port, severity_to_str(ctl.severity), ctl.pid, ctl.msg);

    char _buffer[SYSLOG_MESSAGE_SIZE];
    generate_syslog_message(&ctl, _buffer);

    return 0;
    int sock_fd = 0;
    struct sockaddr_in sa;
    int bytes_sent = 0;
    char buffer[200];
    strcpy(buffer, "<30>Oct 12 12:49:06 host app[12345]: my syslog messssaaaaggggeeee");

    if ((sock_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        fprintf(stderr, "Failed to create a socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr("192.168.1.2");
    sa.sin_port = htons(514);

    if ((bytes_sent = sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr*)&sa, sizeof(sa))) < 0)
    {
        printf("Error sending packet: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    close(sock_fd);

    return 0;
}
