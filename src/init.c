#include "gui.h"
#include "init.h"

char filename[FILENAME_MAX];

void
finalize(void)
{
	free(content.data);

	for (int i = 0; i < NWINDOWS; ++i)
		delwin(win[i]);

	endwin();
}

void
initialize(void)
{
	if (init_ncurses() | init_colors() | init_windows() | init_gui()) {
		endwin();
		fprintf(stderr, "Initializing error.\n");
		exit(EXIT_FAILURE);
	}

	if (LINES < 11 || COLS < 98) {
		endwin();
		fprintf(stderr, "Terminal window is too small.\n"
			"Min: 11x98, your: %dx%d\n", LINES, COLS);
		exit(EXIT_FAILURE);
	}

	wmove(win[EDIT_W], DEFPOS_Y, DEFPOS_X);
}

int
init_colors(void)
{
	if (start_color() == ERR || !has_colors()) {
		fprintf(stderr, "This terminal doesn't support colouring.\n");
		return ERR;
	}

	// Pair id, Foreground, Background
	init_pair(1, COLOR_BLACK, COLOR_YELLOW);	// menu components
	init_pair(2, COLOR_WHITE, COLOR_BLUE);		// edit field
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);	// pop-up windows
	init_pair(4, COLOR_WHITE, COLOR_BLACK);		// borders

	return OK;
}

int
init_gui(void)
{
	for (int i = 0; i < NWINDOWS; ++i)
		draw_window(i);

	return OK;
}

int
init_ncurses(void)
{
	return (initscr() != NULL) & clear() & cbreak() & noecho();
}

int
init_windows(void)
{
	win[MENU_W] = newwin(3, COLS, 0, 0);
	if (win[MENU_W] == NULL) {
		perror("newwin");
		return ERR;
	}

	win[EDIT_W] = newwin(LINES - 8, COLS, 3, 0);
	if (win[EDIT_W] == NULL) {
		perror("newwin");
		return ERR;
	}

	win[INFO_W] = newwin(5, COLS, LINES - 5, 0);
	if (win[INFO_W] == NULL) {
		perror("newwin");
		return ERR;
	}

	// Enable scrolling, func keys, arrows etc.
	keypad(win[EDIT_W], TRUE);

	// Init content
	content.x_pos = DEFPOS_X;
	content.y_pos = DEFPOS_Y;
	content.buf_pos = 0;
	content.size = 0;
	content.data = calloc(BUFSIZ, sizeof(char));

	encryption = FALSE;

	return OK;
}
