
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

    int num1, num2, ans, choice;

S:  bzero(buffer, 255);
    n = read(socketfd, buffer, 255);
    if (n < 0)
        error("Error reading socket");
    printf("server - %s\n", buffer);
    scanf("%d", &num1);
    write(socketfd, &num1, sizeof(int));

    bzero(buffer, 255);
    n = read(socketfd, buffer, 255);
    if (n < 0)
        error("Error reading socket");
    printf("server - %s\n", buffer);
    scanf("%d", &num2);
    write(socketfd, &num2, sizeof(int));

    bzero(buffer, 255);
    n = read(socketfd, buffer, 255);
    if (n < 0)
        error("Error reading socket");
    printf("server - %s\n", buffer);
    scanf("%d", &choice);
    write(socketfd, &choice, sizeof(int));

    if (choice == 5)
        goto Q;

    read(socketfd, &ans, sizeof(int));
    printf("server- The answer is: %d\n", ans);
    
    if (choice != 5)
        goto S;

    Q:
    printf("You have selected to exit.\n");
    close(socketfd);
    return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
