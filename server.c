/*
* this was created using server.c as reference from linuxhowtos.org
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>


void error(const char *msg)
{
    printf("%s", msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockid, newsockid, portno, n, serverChoice;
    socklen_t clilen;
    char buffer[256];
    time_t t;
    if (argc != 2)
    {
        printf("Usage, \"server <port#>\"\n");
        exit(0);
    }
    struct sockaddr_in serv_addr, cli_addr;

    portno = atoi(argv[1]);

    sockid = socket(AF_INET, SOCK_STREAM, 0);
    if (sockid < 0) error("unable to open socket, exiting.\n");

    //sets serv_addr to zero, not sure why, but its in the example.
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //converts the portno to network byte from int byte order.
    serv_addr.sin_port = htons(portno);

    if (bind(sockid, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("Error on binding, exiting\n");

    listen(sockid, 5);
    clilen = sizeof(cli_addr);
    //does a hand shake with the host
    newsockid = accept(sockid, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockid < 0) error("Error on accept, exiting\n");

    printf("starting game!\n");
    while (1)
    {
        //reads from the socket, i.e gets the message from the client
        bzero(buffer, 256);//clears the buffer

        n = read(newsockid, buffer, 255);
        if (n < 0) error("Error reading from socket, exiting\n");
        
        //sets up the rand with a unique seed
        //0 = silent, 1 = betray
        srand(((unsigned) time(&t)));
        serverChoice = rand() % 2;

        if (strcmp(buffer, "exit") == 0) break;
        if (serverChoice == 0 && buffer[0] == 'S')
        {
            bzero(buffer, 256);
            strcpy(buffer, "The server and client both got 1 year in jail\0");
        }
        else if (serverChoice == 0 && buffer[0] == 'B')
        {
            bzero(buffer, 256);
            strcpy(buffer, "The server gets 3 years and the client goes free\0");
        }
        else if (serverChoice == 1 && buffer[0] == 'S')
        {
            bzero(buffer, 256);
            strcpy(buffer, "The server goes free and the client gets 3 years\0");
        }
        else if (serverChoice == 1 && buffer[0] == 'B')
        {
            bzero(buffer, 256);
            strcpy(buffer, "The server and client both get 2 years\0");
        }
        else
        {
            error("Unknown message exiting\n");
        }
        //printf("Here is the message: %s\n", buffer);

        //replies to the client then closes the socket.
        n = write(newsockid, buffer, strlen(buffer));
        if (n < 0) error("Error writing to socket, exiting\n");
    }
    close(newsockid);
    close(sockid);
    return 0;
}