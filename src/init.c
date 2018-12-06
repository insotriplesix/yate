/********************************************************************
 * PROGRAM: yate
 * FILE: init.c
 * PURPOSE: editor initialization functions
 * AUTHOR: 5aboteur <5aboteur@protonmail.com>
 *******************************************************************/

#include "gui.h"
#include "init.h"

/*
 * Function: initialize
 * --------------------
 * Description:
 *  Initializes the editor (run ncurses, enable raw mode, load
 *  configuration file etc).
 *
 * Arguments:
 *  'argc' - number of arguments;
 *  'argv' - list of arguments.
 */

void
initialize(int argc, char *argv[])
{
	if (init_ncurses() || init_colors()) {
		endwin();
		fprintf(stderr, "Ncurses initializing error.\n");
		exit(EXIT_FAILURE);
	}

	if (LINES < 11 || COLS < 98) {
		endwin();
		fprintf(stderr, "Terminal window is too small.\n"
			"Min: 11x98, your: %dx%d\n", LINES, COLS);
		exit(EXIT_FAILURE);
	}

	if (argc > 2) {
		endwin();
		fprintf(stderr, "Usage: ./yate [file]\n");
		exit(EXIT_FAILURE);
	} else if (argc == 2) {
		strcpy(content.info.fname, argv[1]);
		init_content();
		open_file(0);
	} else {
		strcpy(content.info.fname, DEFAULT_FNAME);
		init_content();
	}

	init_windows();
	init_gui();
	get_config();
	load_config();
	enable_raw_mode();

	wmove(win[EDIT_W], DEFPOS_Y, DEFPOS_X);
}

/*
 * Function: finalize
 * ------------------
 * Description:
 *  Restores default terminal attributes, frees allocated memory
 *  and saves current editor configuration into the config file.
 */

void
finalize(void)
{
	disable_raw_mode();

	if (content.nrows != 0)
		free_content();

	for (int i = 0; i < NWINDOWS; ++i)
		delwin(win[i]);

	endwin();
	save_config();
}

/*
 * Function: init_colors
 * ---------------------
 * Description:
 *  Initializes color palette.
 *
 * Returns:
 *  'OK' (0 value) if initialization completed, 'ERR' otherwise.
 */

int
init_colors(void)
{
	if (start_color() == ERR || !has_colors()) {
		fprintf(stderr, "This terminal doesn't support colouring.\n");
		return ERR;
	}

	// Pair id, Foreground, Background
	init_pair(1, COLOR_BLACK, COLOR_YELLOW); // menu components
	init_pair(2, COLOR_WHITE, COLOR_BLUE);   // edit field
	init_pair(3, COLOR_YELLOW, COLOR_BLACK); // pop-up windows
	init_pair(4, COLOR_WHITE, COLOR_BLACK);  // borders

	return OK;
}

/*
 * Function: init_gui
 * ------------------
 * Description:
 *  Draws editor windows.
 */

void
init_gui(void)
{
	for (int i = 0; i < NWINDOWS; ++i) draw_window(i);
}

/*
 * Function: init_ncurses
 * ----------------------
 * Description:
 *  Ncurses library initialization.
 *
 * Returns:
 *  'OK' (0 value) only if all 4 functions return 0, 'ERR' otherwise.
 */

int
init_ncurses(void)
{
	return (initscr() != NULL) & clear() & cbreak() & noecho();
}

/*
 * Function: init_windows
 * ----------------------
 * Description:
 *  Allocates memory for editor windows and enables auxiliary stuff
 *  for user input such as control and function keys.
 *
 * Asserts:
 *  'newwin' won`t return NULL.
 */

void
init_windows(void)
{
	win[MENU_W] = newwin(3, COLS, 0, 0);
	assert(win[MENU_W] != NULL);

	win[EDIT_W] = newwin(LINES - 8, COLS, 3, 0);
	assert(win[EDIT_W] != NULL);

	win[INFO_W] = newwin(5, COLS, LINES - 5, 0);
	assert(win[INFO_W] != NULL);

	// Enable scrolling, func keys, arrows etc
	keypad(win[EDIT_W], TRUE);
}
