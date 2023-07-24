/*
 * 
 * Socket Programming (using fork())
 * A simple Chat Client and Chat Server
 * This is the Client
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
// #include <arpa/inet.h>

#define MAXSIZE 1024
// maxsize = sizeof(buffer)

void join(int groupID)
{
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr;
    struct hostent *server;
    int socket_fd, num, portno;
    char buffer[1024];
    // char* cliname;

    if (argc != 4)
    {
        fprintf(stderr,
                "usage %s <server_host_name> <server_port_number> <client_name>\n",
                argv[0]);
        exit(1);
    }

    portno = atoi(argv[2]);
    if (portno <= 1024)
    {
        fprintf(stderr, "Cannot get host port\n");
        exit(1);
    }

    if ((server = gethostbyname(argv[1])) == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }
    /* Create a socket point */
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR opening socket\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);
    server_addr.sin_addr = *((struct in_addr *)server->h_addr);

    /* Now connect to the server */
    if (connect(socket_fd, (struct sockaddr *)&server_addr,
                sizeof(struct sockaddr)) < 0)
    {
        // fprintf(stderr, "Connection Failure\n");
        perror("ERROR connecting");
        exit(1);
    }

    // buffer = "Hello World!\n";
    // memset(buffer, 0 , sizeof(buffer));
    while (1)
    {

        printf("Client: Enter Data for Server:\n");
        bzero(buffer, MAXSIZE - 1);
        fgets(buffer, MAXSIZE - 1, stdin);

        if (buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't')
        {
            break;
        }

        /*cliname = strdup(argv[3]);
         char *tmp = strdup(cliname);
         strcpy(cliname, "_ClientName:"); //Put str2 or any other string that you want at the beginning
         strcat(cliname, tmp);  //concatenate previous str1
         strcat(cliname, "\n");
         free(tmp);
         if (send(socket_fd, cliname, strlen(argv[3]), 0) == -1) {
         fprintf(stderr, "ERROR Sending Message\n");
         close(socket_fd);
         exit(1);
         }*/

        char *tmp = strdup(buffer);
        strcpy(buffer, argv[3]);
        strcat(buffer, ": ");
        strcat(buffer, tmp);
        free(tmp);

        if ((send(socket_fd, buffer, strlen(buffer), 0)) == -1)
        {
            fprintf(stderr, "ERROR Sending Message\n");
            close(socket_fd);
            exit(1);
        }
        else
        {
            // printf("Client:Message being sent: %s\n", buffer);
            num = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
            if (num < 0)
            {
                printf("Either Connection Closed or Error\n");
                exit(1);
                // Break from the While
                break;
            }

            buffer[num] = '\0';
            printf("Client:Message Received From Server -  %s", buffer);
        }
    }
    close(socket_fd);
    return 0;
} // End of main
