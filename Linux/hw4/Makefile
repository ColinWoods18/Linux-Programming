##################################
# Makefile
# Colin Woods
# COSC 3750 Spring 2022
# Homework 4
# 2/21/22
#
# This file creates the object files prompt.o, display.o, and compute.o
# Then from those object files it generates the approxe executable.
# 
##################################

CC=gcc
CFLAGS=-Wall -ggdb 
RM=/bin/rm -f


.PHONY: wycat clean tidy 

#Generates the wycat executable

wycat: wycat.c
	${CC} ${CFLAGS} wycat.c -o wycat

#Cleans everything up

tidy:
	${RM} a.out core.*
clean: tidy
	${RM} wycat
