/*
wyls.c
Author: Colin Woods
Date: March 2nd, 2022

COSC 3750, Homework 5

This is a simplified version of the ls utility. When given files/links/directories as arguments,
it will print off information about those files, or if it is a directory it will print the info
of all the files/dir/links in that given directory. If no commands are given it will print the
current working directory.

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>



void printFile(char* argv , bool n, bool h) {
   
    struct stat statbuf;
    stat(argv, &statbuf);
    if (S_ISLNK(statbuf.st_mode)) {  
        lstat(argv, &statbuf);
    } 

    //write out the access information
    char access[10];

    for (int i = 0; i < 10; i++){
        access[i] = '-';
    }

    if (S_ISLNK(statbuf.st_mode)) {
        access[0] = 'l';
    }
    else if (S_ISDIR(statbuf.st_mode)) {
        access[0] = 'd';
    }
    if (statbuf.st_mode & S_IRUSR) {
        access[1] = 'r';
    }
    if (statbuf.st_mode & S_IWUSR) {
        access[2] = 'w';
    }
    if (statbuf.st_mode & S_IXUSR) {
        access[3] = 'x';
    }
    if (statbuf.st_mode & S_IRGRP) {
        access[4] = 'r';
    }
    if (statbuf.st_mode & S_IWGRP) {
        access[5] = 'w';
    }
    if (statbuf.st_mode & S_IXGRP) {
        access[6] = 'x';
    }
    if (statbuf.st_mode & S_IROTH) {
        access[7] = 'r';
    }
    if (statbuf.st_mode & S_IWOTH) {
        access[8] = 'w';
    }
    if (statbuf.st_mode & S_IXOTH) {
        access[9] = 'x';
    }
    
    
    // owner name or uid
    char owner[256]; 
    if (n) {
        sprintf(owner, "%d", statbuf.st_uid);
    }
    else {
        struct passwd *pStruct = getpwuid(statbuf.st_uid);
        sprintf(owner, "%s", pStruct->pw_name);
    }
    
    // group name or gid
    char group[256]; 
    if (!n) {
        struct group *gStruct = getgrgid(statbuf.st_gid);
        sprintf(group, "%s", gStruct->gr_name);
    }
    else {
        sprintf(group, "%d", statbuf.st_gid);
    }

    // size of file
    char size[256];
    if (!h || statbuf.st_size < 1000) {
        sprintf(size, "%ld", statbuf.st_size);
    }
    else if (statbuf.st_size < 1000000) {
        sprintf(size, "%.1fK", (double)statbuf.st_size / (1000));
    }
    else if (statbuf.st_size < 1000000000) {
        sprintf(size, "%.1fM", (double)statbuf.st_size / (1000000));
    }
    else {
        sprintf(size, "%.1fG", (double)statbuf.st_size / (1000000000));
    }    
    
    // date and time
    char timeBuf[128] = "";
    time_t now;
    time(&now);
    double timeDiff = difftime(now, statbuf.st_mtime);
    struct tm *time = localtime(&statbuf.st_mtime);
    if (timeDiff < 60*60*24*180) {
        strftime(timeBuf, 128, "%b %e %H:%M", time);
    } else {
        strftime(timeBuf, 128, "%b %e %Y", time);
    }
    
    // print info stats
    printf("%.10s ", access);
    printf("%5s ", owner);
    printf("%5s ", group);
    printf("%10s ", size);
    printf("%10s ", timeBuf);
    printf("%-s \n", argv);

}

void printDir(char *dPath, bool n, bool h) {

    DIR *dir = opendir (dPath);
    if (dir == NULL) {
        perror(dPath);
        return;
    }

  // read each object in the directory
    struct dirent *dp = readdir (dir);
    
    for (; dp != NULL; dp = readdir (dir)) {
    // verify and load file

        char *varName = dp->d_name;
        unsigned char varType = dp->d_type;
    
        if (strcmp(varName, ".") == 0 || strcmp(varName, "..") == 0) {
            continue;
        }

        //create the path name as a char array
        char varPath[1024];
        for(int i = 0; i < 1024; i++){
            varPath[i] = 0;
        }
        strcat(varPath, dPath);
        strcat(varPath, "/");
        strcat(varPath, varName);

        struct stat statbuf;
        if (varType == DT_LNK) {
            lstat(varPath, &statbuf);
        }
        else if (varType == DT_DIR || varType == DT_REG) {
            stat(varPath, &statbuf);
        }
        else {
            printf("%s: current file-type not supported\n", varName);
        }


    
        //write out the access information
        char access[10];

        for (int i = 0; i < 10; i++){
            access[i] = '-';
        }
    
        if (S_ISLNK(statbuf.st_mode)) {
            access[0] = 'l';
        }
        else if (S_ISDIR(statbuf.st_mode)) {
            access[0] = 'd';
        }
        if (statbuf.st_mode & S_IRUSR) {
            access[1] = 'r';
        }
        if (statbuf.st_mode & S_IWUSR) {
            access[2] = 'w';
        }
        if (statbuf.st_mode & S_IXUSR) {
            access[3] = 'x';
        }
        if (statbuf.st_mode & S_IRGRP) {
            access[4] = 'r';
        }
        if (statbuf.st_mode & S_IWGRP) {
            access[5] = 'w';
        }
        if (statbuf.st_mode & S_IXGRP) {
            access[6] = 'x';
        }
        if (statbuf.st_mode & S_IROTH) {
            access[7] = 'r';
        }
        if (statbuf.st_mode & S_IWOTH) {
            access[8] = 'w';
        }
        if (statbuf.st_mode & S_IXOTH) {
            access[9] = 'x';
        }
    
        // owner name or uid
        char owner[256]; 
    
        if (n) {
            sprintf(owner, "%d", statbuf.st_uid);
        }
        else {
            struct passwd *pStruct = getpwuid(statbuf.st_uid);
            sprintf(owner, "%s", pStruct->pw_name);
        }
    
        // group name or gid
        char group[256]; 
        if (!n) {
            struct group *gStruct = getgrgid(statbuf.st_gid);
            sprintf(group, "%s", gStruct->gr_name);
        }
        else {
            sprintf(group, "%d", statbuf.st_gid);
        }
    
        // size of file
        char size[256];
        if (!h || statbuf.st_size < 1000) {
            sprintf(size, "%ld", statbuf.st_size);
        }
        else if (statbuf.st_size < 1000000) {
            sprintf(size, "%.1fK", (double)statbuf.st_size / (1000));
        }
        else if (statbuf.st_size < 1000000000) {
            sprintf(size, "%.1fM", (double)statbuf.st_size / (1000000));
        }
        else {
            sprintf(size, "%.1fG", (double)statbuf.st_size / (1000000000));
        }
    
        // date and time
        char timeBuf[128] = "";
        time_t now;
        time(&now);
        double timeDiff = difftime(now, statbuf.st_mtime);
        struct tm *time = localtime(&statbuf.st_mtime);
        if (timeDiff < 60*60*24*180) {
            strftime(timeBuf, 128, "%b %e %H:%M", time);
        } else {
            strftime(timeBuf, 128, "%b %e %Y", time);
        }
    
        // print info stats
        printf("%.10s ", access);
        printf("%5s ", owner);
        printf("%5s ", group);
        printf("%10s ", size);
        printf("%10s ", timeBuf);
        printf("%-s \n", varPath);
        }

}

int main (int argc, char *argv[]) {

    // Process possible commands
    bool h = false;
    bool n = false;
    int i = 1;

    for (; i < argc; ++i) {
        char *currentChar = argv[i];
        //A command will follow a dash
        if (*currentChar == '-') { 
            currentChar++;
            for (; *currentChar != '\0'; currentChar++) {
                if (*currentChar == 'n') {
                    n = true;
                } else if (*currentChar == 'h') {
                    h = true;
                } else {
                    printf("Command not recognized: -%c\n", *currentChar);
                    return 0;
                }
                
                }
            }
    
        else { 
            // no commands left
            break;
        }
        
        //n and h are set, no files or directories listed after so print working directory
        printDir(".", n, h);
    }

    //if there are no arguments print cwd
    if (argc == 1){
        printDir(".", n, h);
    }

    //otherwise process non-command arguments
  
    while (i < argc) { 
        int isDir = false;

        struct stat statbuf;
        stat(argv[i], &statbuf);

    if (S_ISDIR(statbuf.st_mode)) {
        isDir = true;
    }
    else if ((S_ISLNK(statbuf.st_mode)) || (S_ISREG(statbuf.st_mode))) {
        printFile(argv[i], n, h);
    } 
    else{
        printf("%s: current file-type not supported\n", argv[i]);
    }

    if(isDir){
        printDir(argv[i], n, h);
    } 
    i++;
    }
    
}
