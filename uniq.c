#include "types.h"
#include "stat.h" 
#include "user.h"

#define MAXLINELEN 1024

char buf[512];
// Like strncpy but guaranteed to NUL-terminate.
// tTken from string.c, wasn't able to (read: don't know how to) include it directly
char*
safestrcpy(char *s, const char *t, int n)
{
  char *os;
  
  os = s;
  if(n <= 0)
    return os;
  while(--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  return os;
}

char
tolower(uchar ch){
  if (ch >= 'A' && ch <= 'Z')
    ch = 'a' + (ch - 'A');
  return ch;
}

int
stricmp(const char *p, const char *q)
{
  while(*p && tolower(*p) == tolower(*q))
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

void
uniq(int fd, int flags)
{
  // behavior undefined when line length exceeds MAXLINELEN chars.
  // BUGS: last few lines of input will be IGNORED
  int bytes_read;
  int i;
  int line, character;
  int prev_not_equals_curr;
  int start;
  start = line = character = 0;
  char* prev_line = malloc(MAXLINELEN * sizeof(char));
  char* curr_line = malloc(MAXLINELEN * sizeof(char));

  int cflag, dflag, iflag;
  iflag = flags % 2;
  dflag = (flags / 2)  % 2;
  cflag = (flags / 4) % 2;

  while((bytes_read = read(fd, buf, sizeof(buf))) > 0){
    for(i=0; i < bytes_read; i++){
      curr_line[character] = buf[i];
      character++;
      if(buf[i] == '\n'){
        curr_line[character] = '\0';
        line++;
        
        if (iflag)
          prev_not_equals_curr = stricmp(prev_line, curr_line);
        else
          prev_not_equals_curr = strcmp(prev_line, curr_line);
        
        if(prev_not_equals_curr){
          if (dflag){
            if (line > 1){
              if (cflag && start){
                printf(1,"%d\t", line);
              } else if (cflag){
                start = 1;
              }
            }
            printf(1, prev_line);
            line = 0;
            safestrcpy(prev_line, curr_line, MAXLINELEN);
          }
          else {
            if (cflag && start){
              printf(1,"%d\t", line);
            } else if (cflag){
              start = 1;
            }
            printf(1, prev_line);
            line = 0;
            safestrcpy(prev_line, curr_line, MAXLINELEN);
          }
        }
        character = 0;
      }
    }
  }

  if (dflag){
    if (line > 1){
      if (cflag && start){
        printf(1,"%d\t", line);
      } else if (cflag){
        start = 1;
      }
      printf(1, prev_line);
    }
  }
  else {
    if (cflag && start){
      printf(1,"%d\t", line);
    } else if (cflag){
      start = 1;
    }
    printf(1, prev_line);
  }
  if (line == 1 && !dflag){
    if (cflag && start){
      printf(1,"%d\t", line);
    } else if (cflag){
      start = 1;
    }
    printf(1, curr_line);
  }
  
  free(prev_line);
  free(curr_line);

  if(bytes_read < 0){
    printf(1, "uniq: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;
  int flags = 0;
  if(argc <= 1){
    uniq(0, 0);
    exit();
  }

  for(i = 1; i < argc; i++){
    if(strcmp(argv[i],"-c") == 0){
      flags += 4;
    } else if (strcmp(argv[i],"-d") == 0){
      flags += 2;
    } else if (strcmp(argv[i],"-i") == 0){
      flags += 1;
    } else if((fd = open(argv[i], 0)) < 0){
      printf(1, "uniq: cannot open %s\n", argv[i]);
      exit();
    }

    
  }
  uniq(fd, flags);
  close(fd);
  exit();
}