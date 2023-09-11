/*
 * wyshell.h
 * Colin Woods
 * May, 3, 2022
 * COSC 3750, Homework 10
 *
 * This file holds the utility functions for wyshell.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "wyUtil.h"
#include "wyscanner.h"

struct flagBuf
{
    int semicolon;
    int pipe;
    int amp;
    char redirOut[512];
    char redirIn[512];
    char redirErr[512];
    char redirErrOut[512];
    char appendOut[512];
    char appendErr[512];
    char *argv[512];
};

struct flagBuf *createflagBuf()
{
    struct flagBuf *fb = (struct flagBuf *)malloc(sizeof(struct flagBuf));
    return fb;
}

struct flagBuf *parse(const char *line)
{
    // Init data structure to hold line info
    struct flagBuf *fb = createflagBuf();

    // Flags
    int arg = 0, argc = 0, cc = 0;
    int err = 0;
    int pipe = 0;
    int amp = 0;
    int redirErr = 0, redirError = 0, redirErrOut = 0; 
    int redirOut = 0, redirectOut = 0;
    int redirIn = 0, redirFile = 0;
    int appendError = 0, appendO = 0;

    int rtn = parse_line(line);

    while (rtn != EOL)
    {
        // Amp check - make sure it is last command
        if (amp > 0 && (rtn == WORD || rtn == PIPE || rtn == AMP ||
                        rtn == REDIR_OUT || rtn == REDIR_IN || rtn == APPEND_OUT ||
                        rtn == REDIR_ERR || rtn == APPEND_ERR || rtn == REDIR_ERR_OUT))
        {
            printf("%s\n",
                   "wyshell: syntax error: '&'");
            err = 1;
            break;
        }

        // Determine first word or first argument. Error check first
        if (arg == 0)
        {
            if (rtn == SEMICOLON)
                printf("%s\n", "syntax error: ';'");
            else if (rtn == PIPE)
                printf("%s\n", "syntax error: '|'");
            else if (rtn == AMP && rtn == EOL)
                printf("%s\n", "syntax error: '&'");
            else if (rtn == WORD)
            {
                redirFile = 0;
                fb[cc].argv[0] = malloc(256 * sizeof(char *));
                strcpy(fb[cc].argv[0], lexeme);
                fb[cc].semicolon = 0;
                fb[cc].pipe = 0;
                fb[cc].amp = 0;
                fb[cc].redirErr[0] = '\0';
                fb[cc].redirErrOut[0] = '\0';                
                fb[cc].redirOut[0] = '\0';
                fb[cc].redirIn[0] = '\0';
                fb[cc].appendErr[0] = '\0';
                fb[cc].appendOut[0] = '\0';


                cc++;
                argc = 1;
            }

            if (rtn == SEMICOLON || rtn == PIPE || rtn == AMP)
            {
                err = 1;
                break;
            }
        }
        else
        {
            switch (rtn)
            {
            case WORD:
                if (redirectOut){strcpy(fb[cc - 1].redirOut, lexeme);}
                if (redirIn){strcpy(fb[cc - 1].redirIn, lexeme);}
                if (appendO){strcpy(fb[cc - 1].appendOut, lexeme);}
                if (redirError){strcpy(fb[cc - 1].redirErr, lexeme);}
                if (appendError){
                    strcpy(fb[cc - 1].appendErr, lexeme);}
                if (redirErrOut){strcpy(fb[cc - 1].redirErrOut, lexeme);}

                if (!redirectOut && !redirIn && !appendO && !redirError && !appendError && !redirErrOut)
                {
                    fb[cc - 1].argv[argc] = malloc(256 * sizeof(char *));
                    strcpy(fb[cc - 1].argv[argc], lexeme);
                    argc++;
                }
                redirFile = 0;
                break;
            case SEMICOLON:
                arg = -1;
                fb[cc - 1].semicolon = 1;
                pipe = 0;
                amp = 0;
                redirErr = 0;
                redirError = 0;
                redirErrOut = 0;
                redirOut = 0;
                redirectOut = 0;
                redirIn = 0;
                redirFile = 0;
                appendError = 0;
                appendO = 0;                
                break;
            case PIPE:
                arg = -1;
                pipe = 1;
                fb[cc - 1].pipe = 1;
                redirFile = 1;
                if (redirOut > 0)
                {
                    printf("%s\n\n", "error near pipe");
                    redirFile = 0;
                    err = 1;
                }
                break;
            case AMP:
                fb[cc - 1].amp = 1;
                amp = 1;
                break;
            }
        }
        switch (rtn)
        {
        case REDIR_OUT:
            redirectOut++;
            redirOut++;
            redirFile = 1;
            break;
        case REDIR_IN:
            redirIn++;
            redirFile = 1;
            if (pipe == 1)
            {
                printf("%s\n", "error near pipe");
                err = 1;
            }
            break;
        case APPEND_OUT:
            appendO++;
            redirOut++;
            redirFile = 1;
            break;
        case REDIR_ERR:
            redirError++;
            redirErr++;
            break;
        case APPEND_ERR:
            appendError++;
            redirErr++;
            break;
        case REDIR_ERR_OUT:
            redirErrOut++;
            redirErr++;
            break;
        case QUOTE_ERROR:
            printf("%s\n", "quote error");
            err = 1;
            break;
        case ERROR_CHAR:
            printf("%s%d\n", "character error: ", error_char);
            err = 1;
            break;
        case SYSTEM_ERROR:
            perror("system error");
            exit(1);
            break;
        }
        rtn = parse_line(NULL);
        arg++;
        if (redirOut > 1)
        {
            printf("%s\n", "ambiguous output redirection");
            err = 1;
            redirFile = 0;
            break;
        }

        if (redirIn > 1)
        {
            printf("%s\n", "ambiguous input redirection");
            err = 1;
            redirFile = 0;
            break;
        }

        if (redirErr > 1)
        {
            printf("%s\n", "ambiguous error redirection");
            err = 1;
            redirFile = 0;
            break;
        }

        if (err == 1)
        {
            break;
        }
    }

    if (redirFile != 0)
    {
        printf("%s\n", "error near 'newline'");
        err = 1;
    }

    if (err == 0)
    {
        return fb;
    }
    else
    {
        return NULL;
    }
}


void exec(struct flagBuf *fb)
{
    int i = 0;
    struct flagBuf *pipeCom = (struct flagBuf *)malloc(512 * sizeof(struct flagBuf));
    while (fb[i].argv[0] != NULL)
    {
        int pipecount = 0;
        int rib = 0;
        pid_t pid;

        if (fb[i].pipe)
        {
            while (fb[i].pipe)
            {
                pipeCom[pipecount] = fb[i];
                pipecount++;
                i++;
            }
            pipeCom[pipecount] = fb[i];
        }
        if (fb[i].amp)
        {
            rib = 1;
        }

        if (pipecount < 1)
        {
            if ((pid = fork()) < 0)
                perror("problem forking");
            else if (pid == 0)
            {
                if (!execArg(fb[i]))
                    break;
            }
            else
            {
                // If not running in background, wait.
                if (rib != 1)
                {
                    while (waitpid(-1, NULL, 0) > 0);
                }
            }
        }
        else
        {
            if (!execPipe(pipeCom, pipecount + 1))
            {
                break;
            }
            // If not running in background, wait.
            if (rib != 1)
            {
                while (waitpid(-1, NULL, 0) > 0);
            }
        }

        if (fb[i].semicolon)
        {
            pipecount = 0;
            rib = 0;
            pipeCom = (struct flagBuf *)malloc(512 * sizeof(struct flagBuf));
        }

        i++;
    }
}

int execPipe(struct flagBuf *coms, const int n)
{
    int fd[2];
    pid_t pid;
    int i;

    if (pipe(fd) < 0)
    {
        perror("pipe error");
        return 0;
    }

    if ((pid = fork()) < 0)
    {
        perror("error forking");
        return 0;
    }
    else if (pid == 0)
    {
        close(fd[0]);
        if (dup2(fd[1], STDOUT_FILENO) < 0)
        {
            perror("dup error");
            return 0;
        }
        if (!execArg(coms[0]))
            return 0;
    }

    // loop through but stop before last
    for (i = 1; i < n - 1; i++)
    {
        // fork child n
        if ((pid = fork()) < 0)
        {
            perror("error forking");
            return 0;
        }
        else if (pid == 0)
        {
            close(fd[1]);
            if (dup2(fd[0], STDIN_FILENO) < 0)
            {
                perror("dup error");
                return 0;
            }
            close(fd[0]);
            if (!execArg(coms[n]))
                return 0;
        }
    }

    // fork final child
    if ((pid = fork()) < 0)
    {
        perror("error forking");
        return 0;
    }
    else if (pid == 0)
    {
        close(fd[1]);
        if (dup2(fd[0], STDIN_FILENO) < 0)
        {
            perror("dup error");
            return 0;
        }
        if (!execArg(coms[n - 1]))
            return 0;
    }

    close(fd[0]);
    close(fd[1]);

    return 1;
}

int execArg(struct flagBuf fb)
{
    int in, out;

    // Check for redir out
    if (fb.redirOut[0] != '\0'){
        if ((out = open(fb.redirOut, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0){
            perror(fb.redirOut);
            return 0;
        }
        if (!handleRedir(fb, out))
            return 0;
    }

    // Check for redir in
    if (fb.redirIn[0] != '\0'){
        if ((in = open(fb.redirIn, O_RDONLY)) < 0){
            perror(fb.redirIn);
            return 0;
        }
        if (!handleRedir(fb, in))
            return 0;
    }

    // Check for redir append out
    if (fb.appendOut[0] != '\0'){
        if ((out = open(fb.appendOut, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) < 0){
            perror(fb.appendOut);
            return 0;
        }
        if (!handleRedir(fb, out)) {
            return 0;
        }
    }

    // Check for redir error
    if (fb.redirErr[0] != '\0'){
        if ((out = open(fb.redirErr, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0){
            perror(fb.redirErr);
            return 0;
        }
        if (!handleRedir(fb, out)){
            return 0;
        }
    }

    // Check for redir append error
    if (fb.appendErr[0] != '\0'){
        if ((out = open(fb.appendErr, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR)) < 0){
            perror(fb.appendErr);
            return 0;
        }
        if (!handleRedir(fb, out)){
            return 0;
        }
    }

    // Check for redir error out
    if (fb.redirErrOut[0] != '\0'){
        if ((out = open(fb.redirErrOut, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0){
            perror(fb.redirErrOut);
            return 0;
        }
        if (!handleRedir(fb, out)){
            return 0;
        }
    }

    if (fb.redirOut[0] == '\0' && fb.redirIn[0] == '\0' && fb.appendOut[0] == '\0' && fb.redirErr[0] == '\0' && fb.appendErr[0] == '\0' && fb.redirErrOut[0] == '\0'){
        if (execvp(fb.argv[0], fb.argv) < 0){
            perror(fb.argv[0]); return 0;
        }
    }

    return 1;
}

int handleRedir(struct flagBuf fb, int io)
{
    if (fb.redirOut[0] != '\0' || fb.appendOut[0] != '\0')
    {
        if (dup2(io, STDOUT_FILENO) < 0)
        {
            perror(fb.redirOut);
            return 0;
        }
    }
    else if (fb.redirIn[0] != '\0')
    {
        if (dup2(io, STDIN_FILENO) < 0)
        {
            perror(fb.redirIn);
            return 0;
        }
    }
    else if (fb.redirErr[0] != '\0' || fb.appendErr[0] != '\0' ||
             fb.redirErrOut[0] != '\0')
    {
        if (dup2(io, STDERR_FILENO) < 0)
        {
            if (fb.redirErr[0] != '\0')
                perror(fb.redirErr);
            else if (fb.appendErr[0] != '\0')
                perror(fb.appendErr);
            else
                perror(fb.redirErrOut);
            return 0;
        }
    }

    if (io > 0)
        close(io);

    if (execvp(fb.argv[0], fb.argv) < 0)
    {
        perror(fb.argv[0]);
        return 0;
    }
    return 1;
}
