/*************************************
 * simple.c
 * Author: Kim Buckner
 * Date: 8 April 2020
 *
 * Really simple scanner demo for 3750 wyshell project.
 *
 * MAKEFILE NOTE::::  You WILL get warnings about unused functions in the
 * wyscanner.c file when you use -Wall, unless you turn them off with 
 * -Wno-unused-function
 *
 * The values returned by parse_line() are in the wyscanner.h file. I have just
 * made some strings in an array, because I am lazy, to correspond with those. 
 * The "tokens[rtn&96]" is the way I access. You will see that QUOTE_ERROR has
 * a value of 96 in the header file, and so on. 
 ************************************/

#include<stdio.h>
#include"wyscanner.h"


int main()
{
  char *tokens[]={ "QUOTE_ERROR", "ERROR_CHAR", "SYSTEM_ERROR",
                "EOL", "REDIR_OUT", "REDIR_IN", "APPEND_OUT",
                "REDIR_ERR", "APPEND_ERR", "REDIR_ERR_OUT", "SEMICOLON",
                "PIPE", "AMP", "WORD" };
  int rtn;
  char *rpt;
  char buf[1024];

  while(1) {
    rpt=fgets(buf,256,stdin);
    if(rpt == NULL) {
      if(feof(stdin)) {
	return 0;
      }
      else {
	perror("fgets from stdin");
	return 1;
      }
    }
    rtn=parse_line(buf);
    while(rtn !=  EOL){ 
      // that makes it break out when an error or EOL is seen
      // so no EOL in the switch.
      switch(rtn) {
        case WORD:
          printf("%s\n",lexeme);
          break;
        case ERROR_CHAR:
          printf("%d: %s\t =%d\n",rtn,tokens[rtn%96],error_char);
          break;
        default:
          printf("%d: %s\n",rtn,tokens[rtn%96]);
      }
      rtn=parse_line(NULL);
    }
    printf("\n");
  }
}

