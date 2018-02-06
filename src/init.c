#include "init.h"

char filename[FILENAME_MAX];

/* Init ncurses library */
int
init_ncurses()
{
	return (initscr() != NULL) & clear() & cbreak() & noecho();
}

/* Init color palette */
int
init_colors()
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

/* Init default windows */
int
init_windows()
{
	win[MENU_W] = newwin(3, COLS, 0, 0);

	if (win[MENU_W] == NULL) {
		perror("newwin");
		return ERR;
	}

	wattron(win[MENU_W], COLOR_PAIR(4));
	box(win[MENU_W], ACS_VLINE, ACS_HLINE);
	wattroff(win[MENU_W], COLOR_PAIR(4));
	wbkgd(win[MENU_W], COLOR_PAIR(4));

	int offset = 2;

	wattron(win[MENU_W], COLOR_PAIR(1));
	mvwprintw(win[MENU_W], 1, offset, "  F4 - Open  ");
	offset += 15;
	mvwprintw(win[MENU_W], 1, offset, "  F5 - Save  ");
	offset += 15;
	mvwprintw(win[MENU_W], 1, offset, "  F6 - Extra  ");
	offset += 16;
	mvwprintw(win[MENU_W], 1, offset, "  F7 - Help  ");
	offset += 15;
	mvwprintw(win[MENU_W], 1, offset, "  F8 - Exit  ");
	mvwprintw(win[MENU_W], 1, COLS - 20, " made by 5aboteur ");
	wattroff(win[MENU_W], COLOR_PAIR(1));

	wrefresh(win[MENU_W]);

	win[EDIT_W] = newwin(LINES - 8, COLS, 3, 0);

	if (win[EDIT_W] == NULL) {
		perror("newwin");
		return ERR;
	}

	wattron(win[EDIT_W], COLOR_PAIR(4));
	box(win[EDIT_W], ACS_VLINE, ACS_HLINE);
	wattroff(win[EDIT_W], COLOR_PAIR(4));
	wbkgd(win[EDIT_W], COLOR_PAIR(2));

	wrefresh(win[EDIT_W]);

	win[INFO_W] = newwin(5, COLS, LINES - 5, 0);

	if (win[INFO_W] == NULL) {
		perror("newwin");
		return ERR;
	}

	wattron(win[INFO_W], COLOR_PAIR(4));
	box(win[INFO_W], ACS_VLINE, ACS_HLINE);
	wattroff(win[INFO_W], COLOR_PAIR(4));
	wbkgd(win[INFO_W], COLOR_PAIR(1));

	offset = 4;

	mvwprintw(win[INFO_W], 2, offset, "FILE: %s", filename);
	offset += 10 + strlen(filename);
	mvwprintw(win[INFO_W], 2, offset, "SIZE: %3d b", 0);
	offset += 15;
	mvwprintw(win[INFO_W], 2, offset, "TYPE: %c", 'r');
	offset += 11;
	mvwprintw(win[INFO_W], 2, offset, "ENCRYPT: %c", 'n');
	offset += 14;
	mvwprintw(win[INFO_W], 2, offset, "STATUS: %c", 'n');
	mvwprintw(win[INFO_W], 2, COLS - 10, "v1.0");

	wattron(win[INFO_W], COLOR_PAIR(4));
	mvwprintw(win[INFO_W], 0, COLS / 2 - 6, " %3d : %3d ", 1, 1);

	wrefresh(win[INFO_W]);

	// Enable scrolling, func keys, arrows etc.
	keypad(win[EDIT_W], TRUE);
	wmove(win[EDIT_W], 1, 1);
	scrollok(win[EDIT_W], TRUE);
	idlok(win[EDIT_W], TRUE);
	wrefresh(win[EDIT_W]);

	return OK;
}
