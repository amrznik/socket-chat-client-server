/*
 *
 * Socket Programming (using fork())
 * A simple Chat Client and Chat Server
 * This is the Server
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <errno.h>
#include <string.h>
// #include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3490
#define BACKLOG 10

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr, cli_addr;
    int socket_fd, client_fd, num, portno, clilen;

    socklen_t size;

    char buffer[1024];
    char *groupId[100];
    int nogroup = 0;

    // int nocli = 0;

    memset(buffer, 0, sizeof(buffer));
    int yes = 1;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: server <server_port_number>\n");
        exit(1);
    }

    portno = atoi(argv[1]);
    if (portno <= 1024)
    {
        fprintf(stderr,
                "Please enter higher number (>1024) for server port number\n");
        exit(1);
    }

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "ERROR opening socket\n");
        exit(1);
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if ((bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1)
    {
        // sizeof(struct sockaddr)
        fprintf(stderr, "ERROR on binding\n");
        exit(1);
    }

    if ((listen(socket_fd, BACKLOG)) == -1)
    {
        fprintf(stderr, "Listening Failure\n");
        exit(1);
    }

    while (1)
    {
        size = sizeof(cli_addr);

        if ((client_fd = accept(socket_fd, (struct sockaddr *)&cli_addr, &size)) == -1)
        {
            perror("ERROR on accept");
            exit(1);
        }
        // select();

        printf("Server got connection from client %s\t",
               inet_ntoa(cli_addr.sin_addr));
        printf("Port %d\n", cli_addr.sin_port);
        // nocli++;

        switch (fork())
        {
        case -1:
        {
            perror("ERROR on fork");
            exit(1);
        }
        case 0:
        {
            /* This is the client process */
            close(socket_fd);
            while (1)
            {
                bzero(buffer, strlen(buffer));
                if ((num = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) == -1)
                {
                    perror("ERROR recv");
                    exit(1);
                }
                else if (num == 0)
                {
                    printf("Connection closed %s Port %d\n",
                           inet_ntoa(cli_addr.sin_addr), cli_addr.sin_port);
                    // So we can now wait for another client
                    break;
                }

                if (buffer[0] == 'j' && buffer[1] == 'o' && buffer[2] == 'i' && buffer[3] == 'n')
                {
                    int counter = 5;
                    strcpy(groupId[nogroup], "");
                    while (counter < strlen(buffer))
                    {
                        strcat(groupId[nogroup], &buffer[counter]);
                        counter++;
                    }
                    nogroup++;
                }
                else
                {

                    printf("Server:Msg Received %sFrom %s Port %d\n", buffer,
                           inet_ntoa(cli_addr.sin_addr), cli_addr.sin_port);

                    //     char *tmp = strdup(buffer);
                    //     strcpy(buffer, "I got: "); //Put str2 or any other string that you want at the beginning
                    //     strcat(buffer, tmp);  //concatenate previous str1
                    //     free(tmp);

                    if ((send(client_fd, buffer, strlen(buffer), 0)) == -1)
                    {
                        fprintf(stderr, "ERROR send\n");
                        close(client_fd);
                        exit(1);
                        break;
                    }
                    // printf("Server:Msg being sent: %s\nNumber of bytes sent: %d\n",
                    // buffer, (int) strlen(buffer));
                }
            }

            exit(0);
        }
        default:
        {
            close(client_fd);
        }
        }

        close(client_fd);
    }

    close(socket_fd);
    return 0;
}
