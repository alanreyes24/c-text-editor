#include <termios.h>
#include <unistd.h>
#include <stdlib.h>  


struct termios original_termios;


void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}



void enableRawMode() {

    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disableRawMode);


    struct termios changed_termios = original_termios;

    changed_termios.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &changed_termios);


  
}
int main() {
  enableRawMode();
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
  return 0;
}