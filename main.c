#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_raw;

/*
  revert to original attribs
*/
void disableRawMode(){
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_raw);
}

/*
*/
void enableRawMode(){
  tcgetattr(STDIN_FILENO, &orig_raw);
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
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(){
  enableRawMode();

  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){
    if (iscntrl(c)){
      printf("%d\r\n", c);
    }else {
      printf("%d ('%c')\r\n", c,c);
    }
  }

  return 0;
}