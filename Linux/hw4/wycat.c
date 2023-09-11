/*
wycat.c
Author: Colin Woods
Date: Feb 21st, 2022

COSC 3750, Homework 4

This is the wycat utility. If no arguments are present, it will read from stdin and output to 
stdout. If the argument is a file, it will read the file and output to stdout. If the argument is
a '-' character, it will read from stdin and output to stdout.

*/

#include<stdio.h>
#include<string.h>
#include<errno.h>


int main(int argc, char **argv){
  int ret;
  FILE *inputF;
  char buf[4096];

  if(argc == 1){

    ret = fread(buf, 1, sizeof(buf), stdin);
      if(ret != 4096){
            if(ferror(stdin) != 0){
              perror("Problem reading input");
            }
          }
        clearerr(stdin);  
    fwrite(buf,1,ret,stdout);


  } else{

    int i=1;
    while(i<argc){

      inputF = fopen(argv[i],"r");

      if(strcmp(argv[i], "-") == 0){

        ret = fread(buf, 1, sizeof(buf), stdin);
          if(ret != 4096){
            if(ferror(stdin) != 0){
              perror("Problem reading input");
            }
          }
        clearerr(stdin);  
        fwrite(buf,1,ret,stdout);
      }  
      else if(inputF == NULL){

        perror(argv[i]);

      }
      else{

        ret = fread(buf, 1, sizeof(buf), inputF);
        if(ret != 4096){

          if(ferror(inputF) != 0){
            perror("Cannot read file");

          }
        }

        fwrite(buf,1,ret,stdout);
        fclose(inputF);
      }
      i++;
      }
    }
  }