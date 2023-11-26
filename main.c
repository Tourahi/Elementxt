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
  raw.c_lflag &= ~(ECHO); // set bitflag bits to 0
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(){
  enableRawMode();

  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
  return 0;
}