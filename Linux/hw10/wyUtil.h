/*
* wyshell.h
* Colin Woods
* May, 3, 2022
* COSC 3750, Homework 10
*
* This is the header file for wyUtil.c
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#ifndef _WYUTIL_H
#define _WYUTIL_H

struct flagBuf *createflagBuf();
struct flagBuf *parse(const char *);

void exec(struct flagBuf*);
int execArg(struct flagBuf);
int execPipe(struct flagBuf*, const int);

int handleRedir(struct flagBuf, int);

  #endif