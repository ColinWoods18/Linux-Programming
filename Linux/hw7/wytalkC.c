/*
 * wytalkC.c
 * Colin Woods
 * April 7th, 2022
 *
 * COSC 3750, Homework 7
 *
 * Client for a basic talk app.
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

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("Must give a hostname");
        return 0;
    }

    char *hostname = argv[1];

    // printf("Connecting to %s...\n", hostname);
    int socketDescriptor = request_connection(hostname, 51100);
    if (socketDescriptor == -1)
    {
        return 0;
    }

    FILE *fp = fdopen(socketDescriptor, "r+");
    if (fp == NULL)
    {
        perror("Opening socket");
        return 0;
    }

    while (true)
    {
        if (sendInput(fp) != 0)
        {
            closeSocket(fp);
            break;
        }

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
    }

    return 0;
}