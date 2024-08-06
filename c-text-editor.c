/* includes */

#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/ioctl.h>


/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/* data */

struct editorConfig {
    int screenrows;
    int screencols;
    struct termios original_termios;
};

struct editorConfig E;

struct termios original_termios;

/* output */

void editorDrawRows() {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/* terminal */

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.original_termios) == -1) {
        die("tcsetattr");
    }
}

void enableRawMode() {

    if (tcgetattr(STDIN_FILENO, &E.original_termios) == -1) {
        die("tcgetattr");
    }

    atexit(disableRawMode);

    struct termios changed_termios = E.original_termios;

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
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;
    }
}

/* init */

void initEditor() {
    if (getWindowsSize(&E.screenrows, &E.screencols) == -1) {
        die("getWindowSize");
    }
}

int main() {

    enableRawMode();
    initEditor();

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }

    return 0;
}
