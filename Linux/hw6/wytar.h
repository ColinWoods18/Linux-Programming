/*
 * wytar.h
 * Colin Woods
 * Mar 19, 2022
 * COSC 3750
 * Homework 6
 *
 * This is the wytar header file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <tar.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

#define BLOCK_SIZE 512

struct tar_header
{                       /* byte offset */
    char name[100];     /*   0 */
    char mode[8];       /* 100 */
    char uid[8];        /* 108 */
    char gid[8];        /* 116 */
    char size[12];      /* 124 */
    char mtime[12];     /* 136 */
    char chksum[8];     /* 148 */
    char typeflag;      /* 156 */
    char linkname[100]; /* 157 */
    char magic[6];      /* 257 */
    char version[2];    /* 263 */
    char uname[32];     /* 265 */
    char gname[32];     /* 297 */
    char devmajor[8];   /* 329 */
    char devminor[8];   /* 337 */
    char prefix[155];   /* 345 */
    char pad[12];       /* 500 */
                        /* 512 */
};

int processArg(const char* arg, FILE* archive);
struct tar_header createFile(struct stat file, const char* name);
struct tar_header createLink(struct stat file, const char* name);
struct tar_header createDir(struct stat file, const char* name);

int extractArchive(const char* archName); 
int extractFile(struct tar_header header, FILE* archive); 
int extractLink(struct tar_header header, FILE* archive);
int extractDir(struct tar_header header, FILE* archive); 

int writeHeader(struct tar_header header, FILE* archive); 
int OctToDec(char *);
