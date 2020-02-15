#include "types.h"
#include "stat.h" 
#include "user.h"

#define MAXLINELEN 1024

char buf[MAXLINELEN];
// taken from string.c, wasn't able to (read: don't know how to) include it directly

// copies n bytes from t to s
char*
strncpy(char *s, const char *t, int n)
{
  char *os;
  
  os = s;
  while(n-- > 0 && (*s++ = *t++) != 0)
    ;
  while(n-- > 0)
    *s++ = 0;
  return os;
}


char*
readline(char* dest, char* src, int n)
// reads a line (ending in '\n' or '\0') of at most length buf_len
{
  while (n-- > 0 && *src != '\n' && *src != '\0')
  {
    *dest++ = *src++;
  }
  *dest++ = 0;
  return dest;
}

void
uniq(int fd)
{
  //TODO: error message when line length exceeds 1024
  int bytes_read;
  int i;
  int line, word, character;
  line = word = character = 0;
  char* prev_line = malloc(MAXLINELEN * sizeof(char));
  char* curr_line = malloc(MAXLINELEN * sizeof(char));

  while((bytes_read = read(fd, buf, sizeof(buf))) > 0){
    for(i=0; i < bytes_read; i++){
      curr_line[character] = buf[i];
      character++;
      if(buf[i] == '\n'){
        curr_line[character] = '\0';
        line++;
        if(strcmp(prev_line, curr_line) != 0){
          printf(1, curr_line);
          strncpy(prev_line, curr_line,MAXLINELEN);
        }
        character = 0;
      }
    }
    strncpy(prev_line, curr_line,MAXLINELEN);
  }

  if(bytes_read < 0){
    printf(1, "uniq: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    uniq(0);
    exit();
  }

  for(i = 1; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      printf(1, "uniq: cannot open %s\n", argv[i]);
      exit();
    }
    uniq(fd);
    close(fd);
  }
  exit();
}