/*
 * wytalkD.c
 * Colin Woods
 * April 7th, 2022
 *
 * COSC 3750, Homework 7
 *
 * Server Daemon for a simple talk app.
 *
 */

#include "socketfun.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

int sendInput(FILE *fp)
{
    char buf[BUFFER_SIZE];
    memset(buf, 0, BUFFER_SIZE);

    // Sends the input from the user

    if (fgets(buf, BUFFER_SIZE, stdin) == NULL)
    {
        return -1;
    }

    if (fwrite(buf, 1, strlen(buf), fp) != strlen(buf))
    {
        perror("Error writing to socket");
        return -1;
    }
    return 0;
}

int openSocket(FILE *fp, char *buf)
{

    // Receive the socket and read from the client

    memset(buf, 0, BUFFER_SIZE);
    char *bufIndex = buf;

    if (fgets(bufIndex, BUFFER_SIZE, fp) != NULL)
    {
        return 0;
    }

    return -1;
}

void closeSocket(FILE *fp)
{
    if (fclose(fp) != 0)
    {
        perror("Error closing socket");
    }
}

int main()
{
    char hostname[512];
    gethostname(hostname, 512);

    // I didn't think it was clear what the hostname was so I print a line
    // so we know hostname for wytalkC to use.
    printf("%s\n", hostname);
    if (hostname == NULL)
    {
        perror("Error fetching hostname");
        return 0;
    }

    int socketD = serve_socket(hostname, 51100);
    if (socketD == -1)
    {
        printf("Error serving socket.");
        return 0;
    }

    int fd = accept_connection(socketD);
    if (fd == -1)
    {
        printf("Failure to accept connection");
        return 0;
    }

    FILE *fp = fdopen(fd, "r+");
    if (fp == NULL)
    {
        perror("Error opening socket");
        return 0;
    }

    while (true)
    {
        char buf[BUFFER_SIZE];
        if (openSocket(fp, buf) != 0)
        {
            printf("%s", buf);
            closeSocket(fp);
            break;
        }
        else
        {
            printf("%s", buf);
        }

        if (sendInput(fp) != 0)
        {
            closeSocket(fp);
            break;
        }
    }
}
