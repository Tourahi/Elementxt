/*** includes ***/
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** data ***/
struct termios orig_raw;

void terminate(const char *s){
  perror(s);
  exit(1);
}

/*** term ***/
/*
  revert to original attribs
*/
void disableRawMode(){
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_raw) == -1)
    terminate("tcsetattr at disableRawMode");
}

/*
*/
void enableRawMode(){
  if (tcgetattr(STDIN_FILENO, &orig_raw) == -1) terminate("tcgetattr at enableRawMode");
  atexit(disableRawMode);

  struct termios raw = orig_raw;
  
  tcgetattr(STDIN_FILENO, &raw);
  /*  DISABLE
    ECHO: echo
    ICANON: stop line by line reading
    ISIG: ctrl-s/ctrl-z 
    IEXTEN: ctrl-v
    ICRNL: fix ctrl-m/j (carriage return)
  */
  raw.c_lflag &= ~(ECHO | ICANON | ISIG);
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // turnOff flow ctrl
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) terminate("tcsetattr at enableRawMode");
}

/*** init ***/
int main(){
  enableRawMode();

  while (1){
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) terminate("read at main");

    if (iscntrl(c)){
      printf("%d\r\n", c);
    }else {
      printf("%d ('%c')\r\n", c,c);
    }

    if (c == 'q') break;
  }

  return 0;
}