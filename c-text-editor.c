/* includes */

#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/* data */

struct termios original_termios;

/* terminal */

void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1) {
        die("tcsetattr");
    }
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) {
        die("tcgetattr");
    }

    atexit(disableRawMode);

    struct termios changed_termios = original_termios;

    changed_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    changed_termios.c_oflag &= ~(OPOST);
    changed_termios.c_cflag |= (CS8);
    changed_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);   
    // changed_termios.c_cc[VMIN] = 0;
    // changed_termios.c_cc[VTIME] = 1; 

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &changed_termios) == -1) {
        die("tcsetattr");
    }
}

void charToBinaryString(char c, char *buffer) {
    for (int i = 7; i >= 0; i--) {
        buffer[7 - i] = (c & (1 << i)) ? '1' : '0';
    }
    buffer[8] = '\0';
}

char editorReadKey() {

    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) {
            die("read");
        }
    }
    
    return c;

}

/* input */

void editorProcessKeypress() {

    char c = editorReadKey();

    switch(c) {
        case CTRL_KEY('q'):
        exit(0);
        break;
    }
}

/* init */

int main() {

    enableRawMode();

    while (1) {
        editorProcessKeypress();
    }

    return 0;
}
