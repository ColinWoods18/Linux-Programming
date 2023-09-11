/*
 * wyshell.c
 * Colin Woods
 * April 15, 2021
 *
 * COSC 3750, Homework 8
 *
 * A tokenizer for a shell
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "wyscanner.h"

typedef struct node Node;
typedef struct word Word;

struct node {
    Node *previous, *next;
    char *command;
    Word *arglist;
    int input, output, error;
    char *in_file, *out_file, *err_file;
    int out_file_append, err_file_append;
};

struct word {
    Word *previous, *next;
    char *string;
};

Node *newNode(Node *current) {
    Node *nextNode;

    nextNode = calloc(1, sizeof(Node));
    if (nextNode == NULL) {
        printf("error: Creating new node failed");
        return NULL;
    }

    // default values
    nextNode->input = STDIN_FILENO;
    nextNode->output = STDOUT_FILENO;
    nextNode->error = STDOUT_FILENO;

    if (current != NULL) {
        nextNode->previous = current;
        current->next = nextNode;
    }
    return nextNode;
}

int outputRedirected(Node *n) {
    return (n->output != STDOUT_FILENO || n->out_file != NULL);
}

int inputRedirected(Node *n) {
    return (n->input != STDIN_FILENO || n->in_file != NULL);
}

int errorRedirected(Node *n) {
    return (n->error != STDOUT_FILENO || n->err_file != NULL);
}

void freeWords(Word *head) {
    if (head != NULL) {
        freeWords(head->next);
        free(head);
    }
}

void freeNodes(Node *head) {
    if (head != NULL) {
        freeWords(head->arglist);
        freeNodes(head->next);
        free(head);
    }
}

Word *addWord(Word *current, char *string) {
    Word *nextWord = calloc(1, sizeof(Word));
    if (nextWord == NULL) {
        printf("error: Creating new word failed");
    }
    nextWord->string = string;

    if (current != NULL) {
        // find last arg in list
        Word *tmpWord = current;
        while (tmpWord->next != NULL) {
            tmpWord = tmpWord->next;
        }

        nextWord->previous = tmpWord;
        tmpWord->next = nextWord;
    }

    return nextWord;
}



int main() {
    int rtn;
    char *rpt;
    char buf[1024];

    while (1) {
        printf("$> ");
        rpt = fgets(buf, 256, stdin);
        if (rpt == NULL) {
            if (feof(stdin)) {
                return 0;
            } else {
                perror("fgets from stdin");
                return 1;
            }
        }

        rtn = parse_line(buf);

        // flags to keep track of parsing of input line

        Node *head = newNode(NULL);
        if (head == NULL) {
            continue;
        }
        Node *current = head;

        int finishedParsingLine = 0;
        int pipeFlag = 0;
        int RIB = 0;  // running in background

        int argc = 0;
        char argv[1024][1024];
        int childCount = 0;

        while (!finishedParsingLine) {
            // that makes it break out when an error or EOL is seen

            int finishedParsingCommand = 0;
            int redirFlag = 0;

            while (!finishedParsingCommand && !finishedParsingLine) {
                if (RIB && rtn != EOL) {
                    printf("error: the ampersand must be at the end of line\n");
                    finishedParsingLine = 1;
                    break;
                }

                if (pipeFlag) {
                    if (rtn != WORD) {
                        printf("Must pipe to a command\n");
                        finishedParsingLine = 1;
                        break;
                    }
                    if (current->previous == NULL) {
                        printf("No command to redirect\n");
                        finishedParsingLine = 1;
                        break;
                    }
                    if (outputRedirected(current->previous)) {
                        printf("Ambiguous output\n");
                        finishedParsingLine = 1;
                        break;
                    }
                    if (inputRedirected(current)) {
                        printf("Ambiguous input\n");
                        finishedParsingLine = 1;
                        break;
                    }

                    current->input = PIPE;
                    current->previous->output = PIPE;

                    pipeFlag = 0;
                }

                if (redirFlag) {
                    //printf(" --: %s\n", lexeme);
                    // handle errors
                    if (rtn != WORD) {
                        printf("Must redirect to a file\n");
                        finishedParsingLine = 1;
                        break;
                    }
                    if (current->command == NULL) {
                        printf("No file to redirect\n");
                        finishedParsingLine = 1;
                        break;
                    }

                    switch (redirFlag) {
                        case REDIR_OUT:
                            if (outputRedirected(current)) {
                                printf("Ambiguous output\n");
                                finishedParsingLine = 1;
                                break;
                            }
                            current->out_file = lexeme;
                            break;
                        case REDIR_IN:
                            if (inputRedirected(current)) {
                                printf("Ambiguous input\n");
                                finishedParsingLine = 1;
                                break;
                            }
                            current->in_file = lexeme;
                            break;
                        case APPEND_OUT:
                            if (outputRedirected(current)) {
                                printf("Ambiguous output\n");
                                finishedParsingLine = 1;
                                break;
                            }
                            current->out_file = lexeme;
                            current->out_file_append = 1;
                            break;
                        case REDIR_ERR:
                            if (errorRedirected(current)) {
                                printf("Ambiguous error output\n");
                                finishedParsingLine = 1;
                                break;
                            }
                            current->err_file = lexeme;
                            break;
                        case APPEND_ERR:
                            if (errorRedirected(current)) {
                                printf("Ambiguous error output\n");
                                finishedParsingLine = 1;
                                break;
                            }
                            current->err_file = lexeme;
                            current->err_file_append = 1;
                            break;
                        default:
                            printf("%d not yet implemented\n", redirFlag);
                    }
                    redirFlag = 0;
                } else {  // handle new commands and arguments
                    switch (rtn) {
                        case WORD:
                            strcpy(argv[argc], lexeme);
                            argc++;
                            //I am commenting the print statements out but leaving them as comments because it really helps me
                            //keep track of where I am in the program while coding
                            //printf(" --: %s\n", lexeme);
                            break;
                        case AMP:
                            //printf(" &\n");
                            RIB = 1;
                            break;
                        case REDIR_ERR_OUT:
                            //printf(" 2>&1\n");
                            if (errorRedirected(current)) {
                                printf("Ambiguous error output\n");
                                finishedParsingLine = 1;
                                break;
                            }
                            current->err_file = current->out_file;
                            break;
                        case ERROR_CHAR:
                            printf("error character: %d\n", error_char);
                            finishedParsingLine = 1;
                            break;
                        case SYSTEM_ERROR:
                            perror("system error\n");
                            return 1;
                        case QUOTE_ERROR:
                            printf("quote error\n");
                            finishedParsingLine = 1;
                            break;

                        case PIPE:
                            //printf(" |\n");
                            finishedParsingCommand = 1;
                            pipeFlag = 1;
                            break;
                        case SEMICOLON:
                            //printf(" ;\n");
                            finishedParsingCommand = 1;
                            break;
                        case EOL: ;
                            //printf(" --: EOL\n");
                            pid_t pid = 0;
                            pid = fork();
                            childCount++;
                            if (pid == -1) {
                                perror("Error while forking");
                                return 1;
                            } else if (pid != 0) {
                                if(!RIB){
                                waitpid(pid, 0, 0);
                                }
                                argc = 0;
                            } else {
                                char *arr[1024];
                                for (int i = 0; i < argc; i++) {
                                    arr[i] = argv[i];
                                }
                                arr[argc] = NULL;
                                if (execvp(arr[0], arr)) {
                                    perror("Error executing program");
                                    return 1;
                                }
                                return 0;
                            }
                            //
                            // How can I change this so that it validates the entire command line?
                            //
                            finishedParsingLine = 1;
                            break;
                        default:  // file redirection
                            switch (rtn) {
                                case REDIR_OUT:
                                    //printf(" >\n");
                                    break;
                                case REDIR_IN:
                                    //printf(" <\n");
                                    break;
                                case APPEND_OUT:
                                    //printf(" >>\n");
                                    break;
                                case REDIR_ERR:
                                    //printf(" 2>\n");
                                    break;
                                case APPEND_ERR:
                                    //printf(" 2>>\n");
                                    break;
                            }
                            redirFlag = rtn;
                    }
                }

                rtn = parse_line(NULL);
            }
            // if there is an empty command after a previous command
            if (!finishedParsingLine) {
                if (current->command == NULL && current->previous != NULL) {
                    current = current->previous;
                    free(current->next);
                    current = newNode(current);
                }
                // if the first command is empty
                else if (current->command == NULL &&
                         current->previous == NULL) {
                }
                // normal case
                else {
                    current = newNode(current);
                }
            }
        }
        freeNodes(head);
        for (int ind = 0; ind < childCount; ind ++){
            wait();
        }
        childCount = 0;

    }
}