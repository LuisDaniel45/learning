#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

void error(const char *msg);

int main(int argc, char *argv[])
{
    // validates user input  
    if (argc < 2)
        error("Syntax error: ./port <port> <msg>");


    // intializes var 
    int socketfd, newsockfd, portno, n;

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

    int num1, num2, ans, choice;

    char *prompt = "Enter Number 1:";
S:  n = write(newsockfd, "Enter Number 1:", strlen("Enter Number 1:"));
    if (n < 0)
        error("Error writing to socket");

    read(newsockfd, &num1, sizeof(int));
    printf("Client - Number 1 is: %d\n", num1);


    prompt = "Enter Number 2:";
    n = write(newsockfd, prompt, strlen(prompt));
    if (n < 0)
        error("Error writing to socket");

    read(newsockfd, &num2, sizeof(int));
    printf("Client - Number 2 is: %d\n", num2);

    prompt = "Enter your choice: \n1. Adittion \n2. Substraction \n3. Multiplication \n4. Division \n5. exit \n";
    n = write(newsockfd, prompt, strlen(prompt));
    if (n < 0)
        error("Error writing to socket");

    read(newsockfd, &choice, sizeof(int));
    printf("Client - Choice is: %d\n", choice);

    switch (choice) {
        case 1:
            ans = num1 + num2;
            break;
        case 2:
            ans = num1 - num2;
            break;
        case 3:
            ans = num1 * num2;
            break;
        case 4:
            ans = num1 / num2;
            break;

        case 5:
            goto Q;
            break;
    }

    write(newsockfd, &ans, sizeof(int));
    if (choice != 5)
        goto S;

Q:  close(newsockfd);
    close(socketfd);

    return 0; 
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
