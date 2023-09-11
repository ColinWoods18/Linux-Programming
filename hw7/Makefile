
##################################
# Makefile
# Colin Woods
# COSC 3750 Spring 2022
# Homework 7
# 04/07/2022
#
# This makefile compiles wytalkD and wytalkC
##################################

CC = gcc
CFLAGS = -ggdb -Wall

.PHONY: tidy clean

all: wytalkC wytalkD

wytalkC: wytalkC.c socketfun.c socketfun.h
	${CC} ${CFLAGS} wytalkC.c socketfun.c -o wytalkC

wytalkD: wytalkD.c socketfun.c socketfun.h
	${CC} ${CFLAGS} wytalkD.c socketfun.c -o wytalkD

tidy:
	/bin/rm -f core.* *.o

clean: tidy
	/bin/rm -f wytalkC wytalkD
