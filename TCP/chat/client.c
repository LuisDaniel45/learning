
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

void error(const char *msg);

int main(int argc, char *argv[])
{
    int socketfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[255];
    if (argc < 3)
        error("incorrect input");

    portno = atoi(argv[2]);
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
        error("cannot open socket");
    server = gethostbyname(argv[1]);
    if (server == NULL)
        error("error, no such host");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("connection failed");

    while (1) {
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);
        n = write(socketfd, buffer, strlen(buffer));
        if (n < 0)
            error("error on write");

        bzero(buffer, 255);
        n = read(socketfd, buffer, 255);
        if (n < 0)
            error("error on reading");
        printf("server: %s", buffer);

        int i = strncmp("Bye", buffer, 3);
        if (i == 0)
            break;
    }

    close(socketfd);
    return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
