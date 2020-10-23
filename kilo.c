// kilo.c editor tutorial from https://viewsourcecode.org/snaptoken/kilo/01.setup.html

// chapter 1, setup, the minimal stub
// chapter 2, entering raw mode

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void die(const char *s) {
	perror(s);
	exit(1);
}

void disableRawMode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("disableRawMode-tcsetattr");
}

void enableRawMode() {
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("enableRawMode-tcgetattr");
	atexit(disableRawMode);

	struct termios raw = orig_termios;
	raw.c_cflag |= (CS8);
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_oflag &= ~(OPOST);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("enableRawMode-tcsetattr");
}

int main() {
	enableRawMode();

	while (1) {
		char c = '\0';
		// cygwin returns -1 EAGAIN instead of 0 on read timeout
		if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN)
			die("main-read");
		if (c == '\0') continue;
		if (iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			printf("%d ('%c')\r\n", c, c);
		}
		if (c == 'q') break;
	};
	return 0;
}