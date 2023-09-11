/*
 * wyshell.c
 * Colin Woods
 * May 3, 2022
 *
 * COSC 3750, Homework 10
 *
 * This file contains a shell program that can redirect input/output, pipe, and execute arguments.
 */

#include "wyscanner.h"
#include "wyUtil.h"
#include <stdio.h>



int handleRedir(struct flagBuf, int);
void exec(struct flagBuf *);
int execArg(struct flagBuf);
int execPipe(struct flagBuf *, const int);




int main()
{
    char *rpt;
    char buffer[512];

    // Loop until error or EOF
    while (1)
    {
        printf("%s", "$> ");
        rpt = fgets(buffer, 512, stdin);
        if (rpt == NULL)
        {
            if (feof(stdin))
            {
                return 0;
            }
            else
            {
                perror("error:");
                return 1;
            }
        }

        struct flagBuf *fb = parse(buffer);
        if (fb != NULL)
        {
            exec(fb);
        }
    }
}






