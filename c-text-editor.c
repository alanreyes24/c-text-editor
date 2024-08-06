#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>


struct termios original_termios;


void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void enableRawMode() {

    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disableRawMode);


    struct termios changed_termios = original_termios;

    changed_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    changed_termios.c_oflag &= ~(OPOST);
    changed_termios.c_cflag |= (CS8);
    changed_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);   
    changed_termios.c_cc[VMIN] = 0;
    changed_termios.c_cc[VTIME] = 1; 
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &changed_termios);


  
}
int main() {

  enableRawMode();

  while(1) {

    char c = '\0';
    read(STDIN_FILENO, &c, 1);
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    if (c == 'q') break;



  }
  


  return 0;

  }

