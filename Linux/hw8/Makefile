##################################
# Makefile
# Colin Woods
# COSC 3750 Spring 2022
# Homework 8
# 04/15/2021
#
# This makefile compiles wyshell and wyscanner
##################################

CC = gcc
CFLAGS = -ggdb -Wall

.PHONY: tidy clean

wyshell: wyshell.c wyscanner.c wyscanner.h 
	${CC} ${CFLAGS} wyshell.c wyscanner.c -o wyshell

tidy:
	/bin/rm -f core.* *.o

clean: tidy
	/bin/rm -f wyshell
