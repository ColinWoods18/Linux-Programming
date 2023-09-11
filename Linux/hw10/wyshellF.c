/*
 * wyshell.c
 * Author: Finn Egan
 * Date: May 2, 2022
 *
 * COSC 3750, Homework 10
 *
 * part 3 shell. Code completly rewritten fuck me
 */

#include "wyscanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

struct mem 
{
  int semicolon;
  int pipe;
  int amp;
  char redirOut[256];
  char redirIn[256];
  char appendOut[256];
  char redirErr[256];
  char appendErr[256];
  char redirErrOut[256];
  char *argv[256];
};

extern struct mem *createMem();
extern struct mem *parse(const char*);
extern void exec(struct mem*);
extern int execArg(struct mem);
extern int execPipe(struct mem*, const int);
extern int handleRedir(struct mem, int);

struct mem *createMem() {
  struct mem *m = (struct mem*) malloc(sizeof(struct mem));
  return m;
}

int main()
{
  char *rpt;
  char buf[1024];

  // Loop to parse line after line until error or EOF
  while(1) 
  {
    printf("%s", "$> ");
    rpt=fgets(buf,256,stdin);
    if(rpt == NULL) 
    {
      if(feof(stdin)) 
      {
       return 0;
      }
      else 
      {
       perror("wyshell:");
       return 1;
      }
    }

    struct mem *m = parse(buf);
    if (m != NULL)
    {
      exec(m);
    }
  }
}

struct mem *parse(const char* line) 
{
  // Init data structure to hold line info
  struct mem *m = createMem();
  
  // Variables to keep track of semantics
  int arg = 0;
  int argc = 0;
  int comc = 0;
  int err = 0;
  int redirOut = 0;
  int redirOuto = 0;
  int redirOuta = 0;
  int redirIn = 0;
  int redirErr = 0;
  int redirErre = 0;
  int redirErra = 0;
  int redirErrOut = 0;
  int pipe = 0;
  int redirFile = 0;
  int amp = 0;

  int rtn=parse_line(line);

  while(rtn !=  EOL){
    // Amp check - make sure it is last command
    if (amp > 0 && (rtn == WORD || rtn == PIPE || rtn == AMP || 
      rtn == REDIR_OUT || rtn == REDIR_IN || rtn == APPEND_OUT || 
      rtn == REDIR_ERR || rtn == APPEND_ERR || rtn == REDIR_ERR_OUT)) {
      printf("%s\n",
       "wyshell: syntax error near unexpected token '&'");
      err = 1;
      break;
    }
    
    // Determine first word or first argument. Error check first
    if (arg == 0) 
    {
      if (rtn == SEMICOLON)
        printf("%s\n", "wyshell: syntax error near unexpected token ';'");
      else if (rtn == PIPE)
        printf("%s\n", "wyshell: syntax error near unexpected token '|'");
      else if (rtn == AMP && rtn == EOL)
        printf("%s\n", "wyshell: syntax error near unexpected token '&'");
      else if (rtn == WORD) 
      {
        redirFile = 0;
        m[comc].argv[0] = malloc(256 * sizeof(char*));
        strcpy(m[comc].argv[0], lexeme);
        m[comc].semicolon = 0;
        m[comc].pipe = 0;
        m[comc].amp = 0;
        m[comc].redirOut[0] = '\0';
        m[comc].redirIn[0] = '\0';
        m[comc].appendOut[0] = '\0';
        m[comc].redirErr[0] = '\0';
        m[comc].appendErr[0] = '\0';
        m[comc].redirErrOut[0] = '\0';
        comc++;
        argc = 1;
      }

      if (rtn == SEMICOLON || rtn == PIPE || rtn == AMP)
      {
        err = 1;
        break;
      }
    }
     else 
     {
      switch(rtn) 
      {
        case WORD:
          if (redirOuto) 
          {
            strcpy(m[comc-1].redirOut, lexeme);
          }
          if (redirIn) 
          {
            strcpy(m[comc-1].redirIn, lexeme);
          }
          if (redirOuta) 
          {
            strcpy(m[comc-1].appendOut, lexeme);
          }
          if (redirErre) 
          {
            strcpy(m[comc-1].redirErr, lexeme);
          }
          if (redirErra) 
          {
            strcpy(m[comc-1].appendErr, lexeme);
          }
          if (redirErrOut) 
          {
            strcpy(m[comc-1].redirErrOut, lexeme);
          } 
          if (!redirOuto && !redirIn && !redirOuta && !redirErre && !redirErra && !redirErrOut) 
            {
            m[comc-1].argv[argc] = malloc(256 * sizeof(char*));
            strcpy(m[comc-1].argv[argc], lexeme);
            argc++;
          }
          redirFile = 0;
          break;
        case SEMICOLON:
          arg = -1;
          m[comc-1].semicolon = 1;
          redirOut = 0;
          redirOuto = 0;
          redirOuta = 0;
          redirIn = 0;
          redirErr = 0;
          redirErre = 0;
          redirErra = 0;
          redirErrOut = 0;
          pipe = 0;
          amp =0;
          redirFile = 0;
          break;
        case PIPE:
          arg = -1;
          pipe = 1;
          m[comc-1].pipe = 1;
          redirFile = 1;
          if (redirOut > 0) 
          {
            printf("%s\n\n", "wyshell: redirection error near '|'");
            redirFile = 0;
            err = 1;
          }
          break;
        case AMP:
          m[comc-1].amp = 1;
          amp = 1;
          break;
      }
    }
    switch (rtn) 
    {
      case REDIR_OUT:
        redirOuto++;
        redirOut++;
        redirFile = 1;
        break;
      case REDIR_IN:
        redirIn++;
        redirFile = 1;
        if (pipe == 1) {
          printf("%s\n", "wyshell: redirection error near '|'");
          err = 1;
        }
        break;
      case APPEND_OUT:
        redirOuta++;
        redirOut++;
        redirFile = 1;
        break;
      case REDIR_ERR:
        redirErre++;
        redirErr++;
        break;
      case APPEND_ERR:
        redirErra++;
        redirErr++;
        break;
      case REDIR_ERR_OUT:
        redirErrOut++;
        redirErr++;
        break;
      case QUOTE_ERROR:
        printf("%s\n", "wyshell: quote error");
        err = 1;
        break;
      case ERROR_CHAR:
        printf("%s%d\n", "wyshell: character error: ", error_char);
        err = 1;
        break;
      case SYSTEM_ERROR:
        perror("wyshell: system error");
        exit(1);
        break;
    }
    rtn=parse_line(NULL);
    arg++;
    if (redirOut > 1) 
    {
      printf("%s\n", "wyshell: ambiguous output redirection");
      err = 1;
      redirFile = 0;
      break;
    }

    if (redirIn > 1) 
    {
      printf("%s\n", "wyshell: ambiguous input redirection");
      err = 1;
      redirFile = 0;
      break;
    }

    if (redirErr > 1) 
    {
      printf("%s\n", "wyshell: ambiguous error redirection");
      err = 1;
      redirFile = 0;
      break;
    }

    if (err == 1)
    {
      break;
    }
  }

  if (redirFile != 0) 
  {
    printf("%s\n", "wyshell: syntax error near unexpected token 'newline'");
    err = 1;
  }

  if (err == 0) 
  {
    return m;
  } 
  else 
  {
    return NULL;
  }
}

void exec(struct mem *m) 
{
  int i = 0;
  struct mem *pipeComs = (struct mem*)malloc(256 * sizeof(struct mem));
  while (m[i].argv[0] != NULL) 
  {
    int pipec = 0;
    int bg = 0;
    pid_t pid;

    if (m[i].pipe)
    {
      while (m[i].pipe) 
      {
        pipeComs[pipec] = m[i];
        pipec++;
        i++;
      }
      pipeComs[pipec] = m[i];
    }
    if (m[i].amp)
      bg = 1;

    if (pipec < 1) 
    {
      if ((pid = fork()) < 0)
        perror("yes.");
      else if (pid == 0) 
      {
        if (!execArg(m[i]))
          break;
      } 
      else 
      {
        // If no '&' wait for process to complete
        if (bg != 1)
          while(waitpid(-1, NULL, 0) > 0);
      }
    } 
    else 
    {
      if (!execPipe(pipeComs, pipec+1))
        break;
      // If no '&' wait for process to complete
      if (bg != 1)
        while(waitpid(-1, NULL, 0) > 0);
    }

    if (m[i].semicolon) 
    {
      pipec = 0;
      bg = 0;
      pipeComs = (struct mem*)malloc(256 * sizeof(struct mem));
    }

    i++;
  }
}

int execArg(struct mem m) {
  int in, out;

  // Check for redirection out
  if (m.redirOut[0] != '\0')
  {
    if ((out = open(m.redirOut, O_WRONLY | O_CREAT | O_TRUNC,S_IRUSR | S_IWUSR)) < 0) 
     {
      perror(m.redirOut);
      return 0;
    }
    if (!handleRedir(m, out))
      return 0;
  }

  // Check for redirection in
  if (m.redirIn[0] != '\0'){
    if ((in = open(m.redirIn, O_RDONLY)) < 0) {
      perror(m.redirIn);
      return 0;
    }
    if (!handleRedir(m, in))
      return 0;
  }

  // Check for redirection append out
  if (m.appendOut[0] != '\0'){
    if ((out = open(m.appendOut, O_WRONLY | O_CREAT | O_APPEND, 
          S_IRUSR | S_IWUSR)) < 0) {
      perror(m.appendOut);
      return 0;
    }
    if (!handleRedir(m, out))
      return 0;
  }

  // Check for redirection error
  if (m.redirErr[0] != '\0')
  {
    if ((out = open(m.redirErr, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0) 
     {
      perror(m.redirErr);
      return 0;
    }
    if (!handleRedir(m, out))
      return 0;
  }

  // Check for redirection append error
  if (m.appendErr[0] != '\0')
  {
    if ((out = open(m.appendErr, O_WRONLY | O_CREAT | O_APPEND,
         S_IRUSR | S_IWUSR)) < 0) 
        {
      perror(m.appendErr);
      return 0;
    }
    if (!handleRedir(m, out))
      return 0;
  }

  // Check for redirection error out
  if (m.redirErrOut[0] != '\0')
  {
    if ((out = open(m.redirErrOut, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0) 
            {
      perror(m.redirErrOut);
      return 0;
    }
    if (!handleRedir(m, out))
      return 0;
  }

  if (m.redirOut[0] == '\0' && m.redirIn[0] == '\0' && 
    m.appendOut[0] == '\0' && m.redirErr[0] == '\0' &&
    m.appendErr[0] == '\0' && m.redirErrOut[0] == '\0') 
    {
    if (execvp(m.argv[0], m.argv) < 0) {
      perror(m.argv[0]);
      return 0;
    }
  }

  return 1;
}

int handleRedir(struct mem m, int io) {
  if (m.redirOut[0] != '\0' || m.appendOut[0] != '\0') {
    if (dup2(io, STDOUT_FILENO) < 0) {
      perror(m.redirOut);
      return 0;
    }
  } else if (m.redirIn[0] != '\0') {
    if (dup2(io, STDIN_FILENO) < 0) {
      perror(m.redirIn);
      return 0;
    }
  } else if (m.redirErr[0] != '\0' || m.appendErr[0] != '\0' || 
    m.redirErrOut[0] != '\0') {
    if (dup2(io, STDERR_FILENO) < 0) {
      if (m.redirErr[0] != '\0')
        perror(m.redirErr);
      else if (m.appendErr[0] != '\0')
        perror(m.appendErr);
      else
        perror(m.redirErrOut);
      return 0;
    }
  }

  if (io > 0)
    close(io);

  if (execvp(m.argv[0], m.argv) < 0) {
    perror(m.argv[0]);
    return 0;
  }
  return 1;
}

int execPipe(struct mem *coms, const int n) 
{
  int fd[2];
  pid_t cpid;
  int i;

  // init pipe
  if (pipe(fd) < 0) 
  {
    perror("wyshell");
    return 0;
  }

  // fork child 1
  if ((cpid = fork()) < 0) 
  {
    perror("wyshell");
    return 0;
  } else if (cpid == 0) 
  {
    close(fd[0]);
    if (dup2(fd[1], STDOUT_FILENO)< 0) 
    {
      perror("wyshell");
      return 0;
    }
    if (!execArg(coms[0]))
      return 0;
  }

  // loop through middle children, stop before last child
  for (i = 1; i < n - 1; i++) 
  {
    // fork child n
    if ((cpid = fork()) < 0) 
    {
      perror("wyshell");
      return 0;
    } else if (cpid == 0) 
    {
      close(fd[1]);
      if (dup2(fd[0], STDIN_FILENO)<0)
      {
        perror("wyshell");
        return 0;
      }
      close(fd[0]);
      if (!execArg(coms[n]))
        return 0;
    }
  }

   // fork last child
  if ((cpid = fork()) < 0) 
  {
    perror("wyshell");
    return 0;
  } 
  else if (cpid == 0) 
  {
    close(fd[1]);
    if (dup2(fd[0], STDIN_FILENO)<0) 
    {
      perror("wyshell");
      return 0;
    }
    if (!execArg(coms[n-1]))
      return 0;
  }

  close(fd[0]);
  close(fd[1]);

  return 1;
}
