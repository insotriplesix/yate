#include "editor.h"

char filename[NAME_MAX];

int
change_theme(WINDOW **win, int height, int width)
{
	int fg_field, bg_field, fg_menu, bg_menu, fg_popup, bg_popup;
	int choice = change_theme_popup(height, width);

	if (choice == ERR) return ERR;

	switch ((char)choice) {
		case '0': // default
			fg_menu = COLOR_BLACK, bg_menu = COLOR_YELLOW;
			fg_field = COLOR_WHITE, bg_field = COLOR_BLUE;
			fg_popup = COLOR_YELLOW, bg_popup = COLOR_BLACK;
			break;
		case '1': // leet
			fg_menu = COLOR_BLACK, bg_menu = COLOR_GREEN;
			fg_field = COLOR_GREEN, bg_field = COLOR_BLACK;
			fg_popup = COLOR_GREEN, bg_popup = COLOR_BLACK;
			break;
		case '2': // icy
			fg_menu = COLOR_CYAN, bg_menu = COLOR_BLACK;
			fg_field = COLOR_MAGENTA, bg_field = COLOR_WHITE;
			fg_popup = COLOR_CYAN, bg_popup = COLOR_BLACK;
			break;
		case '3': // hell
			fg_menu = COLOR_BLACK, bg_menu = COLOR_RED;
			fg_field = COLOR_RED, bg_field = COLOR_BLACK;
			fg_popup = COLOR_RED, bg_popup = COLOR_BLACK;
			break;
		default:
			return OK;
	}

	init_pair(1, fg_menu, bg_menu);
	wattrset(win[0], COLOR_PAIR(1));

	init_pair(2, fg_field, bg_field);
	wattrset(win[1], COLOR_PAIR(2));

	init_pair(3, fg_popup, bg_popup);
	wattrset(win[2], COLOR_PAIR(3));

	return OK;
}

int
change_theme_popup(int height, int width)
{
	WINDOW *win;

	int win_height = 6;
	int win_width = 13;
	int line = 1;

	win = newpad(win_height, win_width);

	if (win == NULL) {
		perror("newpad");
		return ERR;
	}

	wattron(win, COLOR_PAIR(4));
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, COLOR_PAIR(4));
	wbkgd(win, COLOR_PAIR(3));

	curs_set(0);

	wmove(win, line++, 1);
	waddstr(win, " [0] def  ");
	wmove(win, line++, 1);
	waddstr(win, " [1] leet ");
	wmove(win, line++, 1);
	waddstr(win, " [2] icy  ");
	wmove(win, line++, 1);
	waddstr(win, " [3] hell ");

	prefresh(win, 0, 0, height / 2 - win_height / 2, width / 2 - win_width / 2,
		height / 2 + win_height / 2, width / 2 + win_width / 2);

	int choice = wgetch(win);

	wclear(win);
	wrefresh(win);
	delwin(win);

	curs_set(1);

	return choice;
}

int
get_help(int height, int width)
{
	return help_popup(height, width);
}

int
help_popup(int height, int width)
{
	WINDOW *win;

	int win_height = 15;
	int win_width = 30;
	int line = 1;

	win = newpad(win_height, win_width);

	if (win == NULL) {
		perror("newpad");
		return ERR;
	}

	wattron(win, COLOR_PAIR(4));
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, COLOR_PAIR(4));
	wbkgd(win, COLOR_PAIR(3));

	curs_set(0);

	wmove(win, line++, win_width / 3);
	waddstr(win, "YATE v1.0");
	wmove(win, line++, 1);
	waddstr(win, "");
	wmove(win, line++, 1);
	waddstr(win, "--- cmd --------------------");
	wmove(win, line++, 1);
	waddstr(win, " F4, ^O - open file");
	wmove(win, line++, 1);
	waddstr(win, " F5, ^K - save to file");
	wmove(win, line++, 1);
	waddstr(win, " F6, ^E - extra options");
	wmove(win, line++, 1);
	waddstr(win, " F7, ^F - get help");
	wmove(win, line++, 1);
	waddstr(win, " F8, ^X - exit");
	wmove(win, line++, 1);
	waddstr(win, "");
	wmove(win, line++, 1);
	waddstr(win, "--- ext --------------------");
	wmove(win, line++, 1);
	waddstr(win, " ^G - change theme");
	wmove(win, line++, 1);
	waddstr(win, " ^Y - live encryption");

	prefresh(win, 0, 0, height / 2 - win_height / 2, width / 2 - win_width / 2,
		height / 2 + win_height / 2, width / 2 + win_width / 2);

	wgetch(win);

	wclear(win);
	wrefresh(win);
	delwin(win);

	curs_set(1);

	return OK;
}

void
open_file(char *buf, int *size, int height, int width)
{
	open_file_popup(height, width);

	FILE *fp;
	char ch;

	if ((fp = fopen(filename, "r"))) {
		while ((ch = fgetc(fp)) != EOF) {
			buf[(*size)++] = ch;
		}

		fclose(fp);
	}
}

char*
open_file_popup(int height, int width)
{
	WINDOW *win;

	char *fname = malloc(sizeof(char) * NAME_MAX);

	int win_height = 3;
	int win_width = 42;

	echo();

	win = newwin(win_height, win_width, height / 2 - win_height / 2,
		width / 2 - win_width / 2);

	wattron(win, COLOR_PAIR(4));
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, COLOR_PAIR(4));
	wbkgd(win, COLOR_PAIR(3));

	mvwaddstr(win, 1, 1, " Enter file name: ");
	wrefresh(win);
	mvwgetstr(win, 1, 19, filename);

	noecho();

	wclear(win);
	wrefresh(win);
	delwin(win);

	return fname;
}

void
save_file(char *buf, int size, int height, int width)
{
	save_file_popup(height, width);

	FILE *fp;
	int i;

	if ((fp = fopen(filename, "w"))) {
		for (i = 0; i < size; ++i) {
			fputc(buf[i], fp);
		}

		fclose(fp);
	}
}

char*
save_file_popup(int height, int width)
{
	WINDOW *win;

	char *fname = malloc(sizeof(char) * NAME_MAX);

	int win_height = 3;
	int win_width = 42;

	echo();

	win = newwin(win_height, win_width, height / 2 - win_height / 2,
		width / 2 - win_width / 2);

	wattron(win, COLOR_PAIR(4));
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, COLOR_PAIR(4));
	wbkgd(win, COLOR_PAIR(3));

	mvwaddstr(win, 1, 1, " Enter file name: ");
	wrefresh(win);
	mvwgetstr(win, 1, 19, filename);

	noecho();

	wclear(win);
	wrefresh(win);
	delwin(win);

	return fname;
}
