###################################################
# Makefile
# Author: Colin Woods
# Date: March 21st, 2022
#
# COSC 3750, Homework 6
#
# This is the makefile to compile the files for homework 6
###################################################

CC=gcc
CFLAGS=-ggdb -Wall

.PHONY: tidy clean

wytar: wytar.c archival.o
	${CC} ${CFLAGS} wytar.c archival.o -o wytar

archival.o: archival.c wytar.h
	${CC} ${CFLAGS} -c archival.c

tidy:
	/bin/rm -rf *.o

clean: tidy
	/bin/rm -rf wytar
