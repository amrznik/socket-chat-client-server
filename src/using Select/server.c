/*
 *
 * Socket Programming (using select())
 * A simple Chat Client and Chat Server
 * This is the Server
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <sys/time.h>
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
    int max_clients = 30;
    int new_socket;
    int sd;
    int socket_fd, client_fd[30], num, portno, GroupID;
    int max_fd;
    int Group[15][15];

    socklen_t size;

    fd_set read_fd;

    char buffer[1024];
    // int NoGroup = 0;

    // initialise all client_socket[] to 0 so not checked
    int i;
    int j;
    int k;
    int CliNamelength;

    for (i = 0; i < max_clients; i++)
    {
        client_fd[i] = 0;
    }

    for (j = 0; j < 15; j++)
    {
        for (k = 0; k < 15; k++)
        {
            Group[j][k] = 100;
        }
    }

    memset(buffer, 0, sizeof(buffer));

    int yes = 1;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: server <server_port_number>\n");
        exit(1);
    }

    portno = atoi(argv[1]);

    // portno = 8000;
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
        close(socket_fd);
        exit(1);
    }

    if ((listen(socket_fd, BACKLOG)) == -1)
    {
        fprintf(stderr, "Listening Failure\n");
        close(socket_fd);
        exit(1);
    }

    while (1)
    {
        size = sizeof(cli_addr);

        FD_ZERO(&read_fd);
        FD_SET(socket_fd, &read_fd);
        max_fd = socket_fd;

        for (i = 0; i < max_clients; i++)
        {
            // socket descriptor
            num = client_fd[i];

            // if valid socket descriptor then add to read list
            if (num > 0)
            {
                FD_SET(num, &read_fd);
                // printf(" fd set i=%d \n", i);
            }
            // highest file descriptor number, need it for the select function
            if (num > max_fd)
            {
                // printf("maxfd=%d \n\r", num);
                max_fd = num;
            }
        }

        num = select(max_fd + 1, &read_fd, NULL, NULL, NULL);

        if ((num < 0))
        {
            perror("ERROR select");
            close(socket_fd);
            // close(client_fd);
            exit(1);
        }

        if ((FD_ISSET(socket_fd, &read_fd)))
        {
            // printf("server socket is triggered!\n\r");
            if ((new_socket = accept(socket_fd, (struct sockaddr *)&cli_addr,
                                     &size)) < 0)
            {
                perror("accept");
                exit(1);
            }
            printf("Server got new connection from client %s\t",
                   inet_ntoa(cli_addr.sin_addr));
            printf("Port %d\n", cli_addr.sin_port);

            for (i = 0; i < max_clients; i++)
            {
                // if position is empty
                if (client_fd[i] == 0)
                {
                    client_fd[i] = new_socket;
                    printf("Adding to list of sockets as %d socket %d\n", i,
                           new_socket);
                    break;
                }
            }
        }

        for (i = 0; i < max_clients; i++)
        {
            sd = client_fd[i];

            if (FD_ISSET(sd, &read_fd))
            {
                // Check if it was for closing , and also read the incoming message
                bzero(buffer, sizeof(buffer));

                if ((num = recv(sd, buffer, sizeof(buffer) - 1, 0)) == -1)
                {
                    perror("ERROR recv");
                    // close(socket_fd);
                    close(sd);
                    client_fd[i] = 0;
                    // close(client_fd);
                    exit(1);
                }
                else if (num == 0)
                {
                    // Somebody disconnected , get his details and print
                    getpeername(sd, (struct sockaddr *)&cli_addr, &size);
                    printf("Client %d disconnected , ip %s , port %d \n",
                           i,
                           inet_ntoa(cli_addr.sin_addr),
                           cli_addr.sin_port);

                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_fd[i] = 0;
                }
                // Echo back the message that came in
                else
                {
                    CliNamelength = buffer[1023] - '0';
                    if (buffer[CliNamelength + 1] == 'j' && buffer[CliNamelength + 2] == 'o' && buffer[CliNamelength + 3] == 'i' && buffer[CliNamelength + 4] == 'n')
                    {
                        GroupID = buffer[CliNamelength + 6] - '0';
                        if (Group[GroupID][i] != 10)
                        {
                            Group[GroupID][i] = 10;
                            printf("Client %d join to Group %d\n", i, GroupID);
                        }
                        else
                        {
                            printf("Cannot Join, Because Client %d is in the Group %d\n", i, GroupID);
                        }
                    }

                    else if (buffer[CliNamelength + 1] == 's' && buffer[CliNamelength + 2] == 'e' && buffer[CliNamelength + 3] == 'n' && buffer[CliNamelength + 4] == 'd')
                    {
                        GroupID = buffer[CliNamelength + 6] - '0';
                        if (Group[GroupID][i] == 10)
                        {
                            for (k = CliNamelength + 1; k < 100; k++)
                            {
                                buffer[k] = buffer[k + 7];
                            }
                            for (j = 0; j < 15; j++)
                            {
                                if (Group[GroupID][j] != 100 && (j != i))
                                {
                                    send(client_fd[j], buffer, strlen(buffer), 0);
                                }
                            }
                            printf("Client %d sent message to Group %d\n", i, GroupID);
                        }
                        else
                        {
                            printf("Cannot Send, Because Client %d is not in the Group %d\n", i, GroupID);
                        }
                    }

                    else if (buffer[CliNamelength + 1] == 'l' && buffer[CliNamelength + 2] == 'e' && buffer[CliNamelength + 3] == 'a' && buffer[CliNamelength + 4] == 'v' && buffer[CliNamelength + 5] == 'e')
                    {
                        GroupID = buffer[CliNamelength + 7] - '0';
                        if (Group[GroupID][i] == 10)
                        {
                            Group[GroupID][i] = 100;
                            printf("Client %d leave Group %d\n", i, GroupID);
                        }
                        else
                        {
                            printf("Cannot Leave, Because Client %d is not in the Group %d\n", i, GroupID);
                        }
                    }

                    else
                    {
                        printf("Message from Client %d: %s", i, buffer);
                        // send(client_fd[0] , buffer , strlen(buffer), 0);
                    }
                }
            }
        }
    }

    close(socket_fd);
    // close(client_fd[i]);
    exit(0);
    return 0;
}
