#include "init.h"

char filename[NAME_MAX];

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
init_windows(WINDOW **win, int height, int width)
{
	// 0 -> Menu bar
	win[0] = newwin(3, width, 0, 0);

	if (win[0] == NULL) {
		perror("newwin");
		return ERR;
	}

	wattron(win[0], COLOR_PAIR(4));
	box(win[0], ACS_VLINE, ACS_HLINE);
	wattroff(win[0], COLOR_PAIR(4));
	wbkgd(win[0], COLOR_PAIR(4));

	int offset = 2;

	wattron(win[0], COLOR_PAIR(1));
	mvwprintw(win[0], 1, offset, "  F4 - Open  ");
	offset += 15;
	mvwprintw(win[0], 1, offset, "  F5 - Save  ");
	offset += 15;
	mvwprintw(win[0], 1, offset, "  F6 - Extra  ");
	offset += 16;
	mvwprintw(win[0], 1, offset, "  F7 - Help  ");
	offset += 15;
	mvwprintw(win[0], 1, offset, "  F8 - Exit  ");
	mvwprintw(win[0], 1, width - 20, " made by 5aboteur ");
	wattroff(win[0], COLOR_PAIR(1));

	wrefresh(win[0]);

	// 1 -> Text field
	win[1] = newwin(height - 8, width, 3, 0);

	if (win[1] == NULL) {
		perror("newwin");
		return ERR;
	}

	wattron(win[1], COLOR_PAIR(4));
	box(win[1], ACS_VLINE, ACS_HLINE);
	wattroff(win[1], COLOR_PAIR(4));
	wbkgd(win[1], COLOR_PAIR(2));

	wrefresh(win[1]);

	// 2 -- Info panel
	win[2] = newwin(5, width, height - 5, 0);

	if (win[2] == NULL) {
		perror("newwin");
		return ERR;
	}

	wattron(win[2], COLOR_PAIR(4));
	box(win[2], ACS_VLINE, ACS_HLINE);
	wattroff(win[2], COLOR_PAIR(4));
	wbkgd(win[2], COLOR_PAIR(1));

	offset = 4;

	mvwprintw(win[2], 2, offset, "FILE: %s", filename);
	offset += 10 + strlen(filename);
	mvwprintw(win[2], 2, offset, "SIZE: %3d b", 0);
	offset += 15;
	mvwprintw(win[2], 2, offset, "TYPE: %c", 'r');
	offset += 11;
	mvwprintw(win[2], 2, offset, "ENCRYPT: %c", 'n');
	offset += 14;
	mvwprintw(win[2], 2, offset, "STATUS: %c", 'n');
	mvwprintw(win[2], 2, width - 10, "v1.0");

	wattron(win[2], COLOR_PAIR(4));
	mvwprintw(win[2], 0, width / 2 - 6, " %3d : %3d ", 1, 1);

	wrefresh(win[2]);

	// Enable func keys, arrows etc.
	keypad(win[1], TRUE);
	wmove(win[1], 1, 1);

	wrefresh(win[1]);

	return OK;
}
