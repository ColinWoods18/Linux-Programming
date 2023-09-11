##################################
# Makefile
# Colin Woods
# COSC 3750 Spring 2022
# Homework 3
# 2/8/22
#
# This file creates the object files prompt.o, display.o, and compute.o
# Then from those object files it generates the approxe executable.
# 
##################################

CC=gcc
CFLAGS=-Wall -ggdb -I .
RM=/bin/rm -f
OBJS=prompt.o display.o compute.o


.PHONY: approxe clean tidy 

#Generates the approxe executable

approxe: ${OBJS}
	${CC} ${CFLAGS} approxe.c ${OBJS} -o approxe

#Generates the object files

prompt: prompt.c prompt.h
	${CC} -c ${CFLAGS} prompt.c -o prompt 
 
compute: compute.c compute.h
	${CC} -c ${CFLAGS} compute.c -o compute	

display: display.c display.h
	${CC} -c ${CFLAGS} display.c -o display

#Cleans everything up

tidy:
	${RM} a.out core.*
clean: tidy
	${RM} prompt.o compute.o display.o approxe
