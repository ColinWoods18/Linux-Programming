##################################
# Makefile
# Colin Woods
# COSC 3750 Spring 2022
# Homework 5
# 3/2/22
#
# This makefile creates the wyls executable.
# 
##################################

CC=gcc
CFLAGS=-Wall -ggdb 
RM=/bin/rm -f


.PHONY: clean tidy 

#Generates the wyls executable

wyls: wyls.c
	${CC} ${CFLAGS} wyls.c -o wyls

#Cleans everything up

tidy:
	${RM} a.out core.*
clean: tidy
	${RM} wyls
