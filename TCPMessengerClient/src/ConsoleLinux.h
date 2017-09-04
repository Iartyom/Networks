#include <stdio.h>

#define ANSI_COLOR_NORMAL  "\x1b[0m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"

enum consoleColor {
	COL_NORMAL = 0,
	COL_RED,
	COL_GREEN,
	COL_YELLOW,
	COL_BLUE,
	COL_MAGENTA,
	COL_CYAN,
};

class ConsoleLinux {
public:
	ConsoleLinux() {
		//Sprintf("\033[r");
	}
	~ConsoleLinux() {
	}

	void setCursorPosition(int row, int col) {
		printf("\033[%d;%dH", row, col);
	}

	void clearScreen() {
		printf("\033[2J");
		fflush(stdout);
	}

	void clearLine() {
		//printf("\033[K");
		//printf("\33[2K");
		printf("\r                  \r");
		fflush(stdout);
	}

	const char *codeToColor(consoleColor col) {
		switch (col) {
		default:
		case COL_NORMAL:
			return ANSI_COLOR_NORMAL;
		case COL_RED:
			return ANSI_COLOR_RED;
		case COL_GREEN:
			return ANSI_COLOR_GREEN;
		case COL_YELLOW:
			return ANSI_COLOR_YELLOW;
		case COL_BLUE:
			return ANSI_COLOR_BLUE;
		case COL_MAGENTA:
			return ANSI_COLOR_MAGENTA;
		case COL_CYAN:
			return ANSI_COLOR_CYAN;
		}
	}

	void setColor(consoleColor col) {
		printf("%s", codeToColor(col));
	}

	void updateScreen(void) {
		printf("\033[H");
		fflush(stdout);
	}
};
