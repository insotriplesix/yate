#include "editor.h"

char filename[FILENAME_MAX];

int
change_theme()
{
	int fg_field, bg_field,
		fg_menu, bg_menu,
		fg_popup, bg_popup;

	int choice = change_theme_popup();
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
	wattrset(win[MENU_W], COLOR_PAIR(1));

	init_pair(2, fg_field, bg_field);
	wattrset(win[EDIT_W], COLOR_PAIR(2));

	init_pair(3, fg_popup, bg_popup);
	wattrset(win[INFO_W], COLOR_PAIR(3));

	return OK;
}

int
change_theme_popup()
{
	WINDOW *win;

	int win_height = 6;
	int win_width = 13;
	int line = 1;

	int offset_y = LINES / 2;
	int offset_x = COLS / 2;

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

	prefresh(win, 0, 0,
		offset_y - win_height / 2,
		offset_x - win_width / 2,
		offset_y + win_height / 2,
		offset_x + win_width / 2);

	int choice = wgetch(win);

	wclear(win);
	wrefresh(win);
	delwin(win);

	curs_set(1);

	return choice;
}

int
get_extra()
{
	WINDOW *win;

	int win_height = 4;
	int win_width = 14;
	int line = 0;

	int offset_y = 2;
	int offset_x = 32;

	win = newpad(win_height, win_width);

	if (win == NULL) {
		perror("newpad");
		return ERR;
	}

	wbkgd(win, COLOR_PAIR(1));

	curs_set(0);

	wmove(win, line++, 1);
	waddstr(win, "------------");
	wmove(win, line++, 1);
	waddstr(win, "^W - hexedit");
	wmove(win, line++, 1);
	waddstr(win, "^Y - livexor");
	wmove(win, line++, 1);
	waddstr(win, "^G - themez");

	prefresh(win, 0, 0,
		offset_y, offset_x,
		offset_y + win_height,
		offset_x + win_width);

	wgetch(win);

	wclear(win);
	wrefresh(win);
	delwin(win);

	curs_set(1);

	return OK;
}

int
get_help()
{
	WINDOW *win;

	int win_height = 15;
	int win_width = 30;
	int line = 1;

	int offset_y = LINES / 2;
	int offset_x = COLS / 2;

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

	prefresh(win, 0, 0,
		offset_y - win_height / 2,
		offset_x - win_width / 2,
		offset_y + win_height / 2,
		offset_x + win_width / 2);

	wgetch(win);

	wclear(win);
	wrefresh(win);
	delwin(win);

	curs_set(1);

	return OK;
}

int
open_file(char **buf, int *size, bool from_arg)
{
	if (!from_arg && open_file_popup() == ERR) {
		perror("open_file_popup");
		return ERR;
	}

	FILE *fp = fopen(filename, "r");

	if (fp == NULL) {	// MOVE2 INFO BAR
		fprintf(stderr, "Error opening file '%s', skipped.", filename);
		return OK;
	}

	// Get the file length
	fseek(fp, 0L, SEEK_END);
	long len = ftell(fp);
	rewind(fp);

	if (len > BUFSIZ && len > (long)*size) {
		// Allocate more space for the buffer
		char *tmp = realloc(*buf, sizeof(char) * len + 1);

		if (tmp == NULL) {
			perror("realloc");
			fclose(fp);
			return ERR;
		}

		*buf = tmp;
	}

	char ch;
	int i = 0;

	while ((ch = fgetc(fp)) != EOF)
		(*buf)[i++] = ch;

	(*buf)[i] = '\0';

	*size = i;
	fclose(fp);

	return OK;
}

int
open_file_popup()
{
	WINDOW *win;

	int win_height = 3;
	int win_width = 42;

	int offset_y = LINES / 2;
	int offset_x = COLS / 2;

	win = newwin(win_height, win_width,
		offset_y - win_height / 2,
		offset_x - win_width / 2);

	if (win == NULL) {
		perror("newwin");
		return ERR;
	}

	wattron(win, COLOR_PAIR(4));
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, COLOR_PAIR(4));
	wbkgd(win, COLOR_PAIR(3));

	echo();

	mvwaddstr(win, 1, 1, " Enter file name: ");
	wrefresh(win);
	memset(filename, '\0', FILENAME_MAX);
	scrollok(win, TRUE);
	idlok(win, TRUE);
	nonl();
	mvwgetstr(win, 1, 19, filename);

	noecho();

	wclear(win);
	wrefresh(win);
	delwin(win);

	return OK;
}

int
save_file(char *buf, int size)
{
	if (save_file_popup() == ERR) {
		perror("save_file_popup");
		return ERR;
	}

	FILE *fp = fopen(filename, "w");

	if (fp == NULL) {	// MV2INFOBAR
		fprintf(stderr, "Error opening file '%s', skipped.", filename);
		return OK;
	}

	for (int i = 0; i < size; ++i)
		fputc(buf[i], fp);

	fclose(fp);

	return OK;
}

int
save_file_popup()
{
	WINDOW *win;

	int win_height = 3;
	int win_width = 42;

	int offset_y = LINES / 2;
	int offset_x = COLS / 2;

	win = newwin(win_height, win_width,
		offset_y - win_height / 2,
		offset_x - win_width / 2);

	if (win == NULL) {
		perror("newwin");
		return ERR;
	}

	wattron(win, COLOR_PAIR(4));
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, COLOR_PAIR(4));
	wbkgd(win, COLOR_PAIR(3));

	echo();

	mvwaddstr(win, 1, 1, " Enter file name: ");
	wrefresh(win);
	memset(filename, '\0', FILENAME_MAX);
	mvwgetstr(win, 1, 19, filename);

	noecho();

	wclear(win);
	wrefresh(win);
	delwin(win);

	return OK;
}
