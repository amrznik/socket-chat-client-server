/*
 *
 * Socket Programming (using select())
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

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, cli_addr;
    struct hostent *server;
    int socket_fd, client_fd, max_fd, sd, num, portno;
    int new_socket;
    char buffer[1024], CliNameBuff[1024];
    char server_reply[1024];
    // char* cliname;
    fd_set read_fd;
    socklen_t size;

    if (argc != 4)
    {
        fprintf(stderr,
                "usage %s <server_host_name> <server_port_number> <client_name>\n",
                argv[0]);
        exit(1);
    }

    client_fd = 0;

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

    memset(buffer, 0, sizeof(buffer));
    memset(CliNameBuff, 0, sizeof(CliNameBuff));
    memset(server_reply, 0, sizeof(server_reply));
    sd = socket_fd;

    int i;
    for (i = 0; i < strlen(argv[3]); i++)
    {
        CliNameBuff[i] = argv[3][i];
    }
    CliNameBuff[strlen(argv[3])] = ':';
    CliNameBuff[strlen(argv[3]) + 1] = ' ';

    CliNameBuff[1023] = (char)(((int)'0') + strlen(argv[3]) + 2);

    while (1)
    {
        size = sizeof(cli_addr);

        FD_ZERO(&read_fd);
        FD_SET(0, &read_fd);

        printf("%s: Enter Data for Server:\n", argv[3]);

        max_fd = 0;

        FD_SET(socket_fd, &read_fd);

        // highest file descriptor number, need it for the select function
        if (socket_fd > max_fd)
        {
            max_fd = socket_fd;
        }

        num = select(max_fd + 1, &read_fd, NULL, NULL, NULL);

        if ((num < 0))
        {
            perror("ERROR in Client select");
            // close(socket_fd);
            // close(client_fd);
            exit(1);
        }

        if ((FD_ISSET(0, &read_fd)))
        {
            bzero(buffer, MAXSIZE - 1);
            bzero(CliNameBuff, MAXSIZE - 1);
            fgets(buffer, MAXSIZE - 1, stdin);

            // strcat(CliNameBuff, buffer);

            // for (i = 0; i < 1000; i++) {
            //	buffer[i + (strlen(argv[3])) + 2] = buffer[i];
            // }

            for (i = 0; i < 900; i++)
            {
                CliNameBuff[i + strlen(argv[3]) + 2] = buffer[i];
            }

            if (buffer[0] == 'q' && buffer[1] == 'u' && buffer[2] == 'i' && buffer[3] == 't')
            {
                break;
            }

            if ((send(socket_fd, CliNameBuff, strlen(CliNameBuff), 0)) == -1)
            {
                fprintf(stderr, "ERROR Sending Message\n");
                close(socket_fd);
                exit(1);
            }
            else
            {
                printf("%s:Message being sent\n", argv[3]);
            }
        }

        if (FD_ISSET(sd, &read_fd))
        {
            // Check if it was for closing , and also read the incoming message
            bzero(server_reply, sizeof(server_reply));

            if ((num = recv(sd, server_reply, sizeof(server_reply) - 1, 0)) == -1)
            {
                perror("ERROR recv");
                // close(socket_fd);
                close(sd);
                client_fd = 0;
                // close(client_fd);
                exit(1);
            }
            else if (num == 0)
            {
                // Somebody disconnected , get his details and print
                getpeername(sd, (struct sockaddr *)&cli_addr, &size);
                printf("Host disconnected , ip %s , port %d \n",
                       inet_ntoa(cli_addr.sin_addr),
                       cli_addr.sin_port);

                // Close the socket and mark as 0 in list for reuse
                close(sd);
                client_fd = 0;
            }
            else
            {
                printf("Client: %s", server_reply);
            }
        }
    }

    close(socket_fd);
    return 0;
}
