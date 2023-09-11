/*
 * wytar.c
 * Colin Woods
 * Kim Buckner
 * March 23, 2022
 *
 * COSC 3750, Homework 6
 *
 * This file is part of a simplified version of the tar utility
 * 
 */

#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <tar.h>
#include <unistd.h>

#include "wytar.h"


int main(int argc, char** argv) {

    int nextFileIndex = 0;
    bool c = false;
    bool x = false;
    char archiveName[4096];

    int i = 1;
    for (; i < argc; i++)
    {
        //check for and process command input:
        if (i == nextFileIndex)
        {
            continue;
        }
        char *currentChar = argv[i];
        if (*currentChar == '-')
        {
            currentChar++;
            for (; *currentChar != '\0'; currentChar++)
            {
                if (*currentChar == 'c')
                {
                    if (x)
                    {
                        printf("Cannot create and extract an archive.\n");
                        return 0;
                    }
                    else
                    {
                        c = true;
                    }
                }
                else if (*currentChar == 'x')
                {
                    if (c)
                    {
                        printf("Cannot create and extract an archive.\n");
                        return 0;
                    }
                    else
                    {
                        x = true;
                    }
                }
                else if (*currentChar == 'f')
                {
                    nextFileIndex = i + 1;
                    strcpy(archiveName, argv[i+1]);
                }
                else
                {
                    printf("Command not recognized: -%c\n", *currentChar);
                    return 0;
                }
            }
        }
        else
        { 
            break;
        }
    }

    if (c == false && x == false) 
    {
        printf("Must select a command.\n");
        return 0;
    }
    if (nextFileIndex == 0 || nextFileIndex == argc)
    {
        printf("Must be specify a filename \n");
        return 0;
    }

    if (c && !x)
    {
        // Create an archive
        FILE* archive = fopen(archiveName, "w+");
        if (archive == NULL) {
            perror(archiveName);
            return -1;
        }

       for (; i < argc; i++) {
            if (!processArg(argv[i], archive)) {
                printf("Archive creation failed\n");
                return -1;
            }
         
        }

        char temp[512];
        memset(temp, 0, 512);
        fwrite(&temp, sizeof(char), 512, archive);
        fwrite(&temp, sizeof(char), 512, archive);
        fclose(archive);

        
    }
    else if (x && !c)
    {
        // Extract the archive
        if (!extractArchive(archiveName)) {
            printf("%s\n", "Unable to extract archive");
            return -1;
        }
    } 
    return 0;
}
