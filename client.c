/*
* this was created using client.c as reference from linuxhowtos.org
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

void error(const char *msg)
{
    printf("%s", msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    signal (SIGPIPE, SIG_IGN);
    if (argc < 3)
    {
        error("Usage, \"client <server addr> <port#>\n");
    }

    int sockid, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    portno = atoi(argv[2]);
    sockid = socket(AF_INET, SOCK_STREAM, 0);

    if (sockid < 0) error("Error opening socket\n");

    server = gethostbyname(argv[1]);
    if (server == NULL) error("No such host!\n");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockid, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("Error connecting");
    while (1)
    {
        printf("\nYou and your partner in crime are being interrogated!!\n");
        printf("Enter 'S' to stay silent or 'B' to betray: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        printf("%s\n", buffer);
        n = write(sockid, buffer, strlen(buffer));

        if (n < 0) error("Error writing to socket");

        bzero(buffer, 256);
        n = read(sockid, buffer, 255);

        if (n < 0) error("Error reading from socket");
        printf("%s\n\n", buffer);

        printf("Would you like to try again?[Y/N]: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        if (buffer[0] == 'Y' || buffer[0] == 'y')
        {
            continue;
        }
        else
        {
            bzero(buffer, 256);
            strcpy(buffer, "exit\0");
            write(sockid, buffer, strlen(buffer));
            break;
        }
    }
    close(sockid);
    return 0;
}