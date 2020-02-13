#include "types.h"
#include "stat.h" 
#include "user.h"
// is it possible to include string.c without modifying user.h/creating our own string.h?

char buf[512];
char prev_line[512];
char curr_line[512];

// Like strncpy but guaranteed to NUL-terminate.
// Taken from xv6 string.c
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

void
uniq(int fd)
{
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0){
    // TODO: split lines in buf, put into curr_line (line 34)
    // TODO: thats not how strchr works T_T
    safestrcpy(curr_line, buf, strchr(buf,'\n') + 1);
    if (strcmp(prev_line, curr_line) != 0) { 
      printf(1, "DEBUG\n");
      printf(1, "prev=%s\n", prev_line);
      printf(1, "curr=%s\n", curr_line);
      printf(1, "END DEBUG\n");
      write(1, buf, n);
      safestrcpy(prev_line, curr_line, sizeof(curr_line));
    }
  }
  if(n < 0){
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