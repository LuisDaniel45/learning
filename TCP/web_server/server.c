#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>

void error(const char *msg);

int main(int argc, char *argv[])
{
    // validates user input  
    if (argc < 2)
        error("Syntax error: ./port <port> <msg>");


    // intializes var 
    int socketfd, newsockfd, portno, n;
    char buffer[255];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen; 

    // creates socket and makes sure it can be opened 
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("error opening socket");

    // cleans buffer
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]); // change string to int 

    // gives values to 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            error("binding error");

    listen(socketfd, 5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(socketfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("Error on accept");

    while (1) {
        bzero(buffer, 255);
        n = read(newsockfd, buffer, 255);
        printf("client: \n%s\n", buffer);
        if (buffer[n - 1] == '\n')
            break;
    }
    if (n < 0)
        error("Error reading client");

    char *rensponse = "HTTP/1.0 200 OK\r\nContent-Type:text/html\r\n\r\n";
    n = write(newsockfd, rensponse, strlen(rensponse));
    if (n < 0)
        error("Error responding to client");

    FILE *fp;
    fp = fopen("template.html", "r");
    if (fp == NULL)
        error("Error opening file");

    bzero(buffer, 255);
    while (fgets(buffer, 255, fp) != NULL) {
        n = write(newsockfd, buffer, 255);
        printf("%s", buffer);
        if (n < 0)
            error("Error sending html");
        bzero(buffer, 255);

    }

    n = read(newsockfd, buffer, 255);
    printf("client: \n%s\n", buffer);

    fclose(fp);
    close(newsockfd);
    close(socketfd);

    return 0; 
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
