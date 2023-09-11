##################################
# Makefile
# Colin Woods
# COSC 3750 Spring 2022
# Homework 10
# 05/03/2021
#
# This makefile compiles wyshell, wyUtil and wyscanner
##################################

CC = gcc
CFLAGS = -ggdb -Wall

.PHONY: tidy clean

wyshell: wyshell.c wyscanner.c wyscanner.h wyUtil.c wyUtil.h 
	${CC} ${CFLAGS} wyshell.c wyscanner.c wyUtil.c -o wyshell

tidy:
	/bin/rm -f core.* *.o

clean: tidy
	/bin/rm -f wyshell
