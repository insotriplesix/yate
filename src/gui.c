#include "gui.h"

void
draw_edit(enum win_t wtype)
{
	wbkgd(win[wtype], EDIT_CLR);
}

void
draw_info(enum win_t wtype)
{
	wbkgd(win[wtype], MENU_CLR);

	int offset = 4;

	mvwprintw(win[wtype], 2, offset, "FILE: %s", content.file.name);
	offset += 10 + strlen(content.file.name);
	mvwprintw(win[wtype], 2, offset, "SIZE: %lld b", content.file.size);
	offset += 15;
	mvwprintw(win[wtype], 2, offset, "MODE: %lo", content.file.mode);
	offset += 16;
	mvwprintw(win[wtype], 2, offset, "ENCRYPT: %c", encryption ? 'y' : 'n');
	offset += 14;
	mvwprintw(win[wtype], 2, offset, "ERROR: %c", 'n');
	mvwprintw(win[wtype], 2, COLS - 10, "v1.1b");

	wattron(win[wtype], BORDER_CLR);
	mvwprintw(win[wtype], 0, COLS / 2 - 6, " %3d : %3d ", 1, 1);
}

void
draw_menu(enum win_t wtype)
{
	wbkgd(win[wtype], BORDER_CLR);

	int offset = 2;

	wattron(win[wtype], MENU_CLR);
	mvwprintw(win[wtype], 1, offset, "  F4 - Open  ");
	offset += 15;
	mvwprintw(win[wtype], 1, offset, "  F5 - Save  ");
	offset += 15;
	mvwprintw(win[wtype], 1, offset, "  F6 - Extra  ");
	offset += 16;
	mvwprintw(win[wtype], 1, offset, "  F7 - Help  ");
	offset += 15;
	mvwprintw(win[wtype], 1, offset, "  F8 - Exit  ");
	mvwprintw(win[wtype], 1, COLS - 20, " made by 5aboteur ");
	wattroff(win[wtype], MENU_CLR);
}

void
draw_window(enum win_t wtype)
{
	wattron(win[wtype], BORDER_CLR);
	box(win[wtype], ACS_VLINE, ACS_HLINE);
	wattroff(win[wtype], BORDER_CLR);

	switch (wtype) {
		case MENU_W:
			draw_menu(wtype);
			break;
		case EDIT_W:
			draw_edit(wtype);
			break;
		case INFO_W:
			draw_info(wtype);
			break;
		default:
			break;
	}

	wrefresh(win[wtype]);
}

int
change_theme_ed(void)
{
	int theme = change_theme_popup();
	if (theme == ERR) return ERR;

	return change_theme(theme);
}

int
change_theme(int theme)
{
	int fg_field, bg_field,
		fg_menu, bg_menu,
		fg_popup, bg_popup;

	current_theme = (char)theme;

	switch ((char)theme) {
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
	wattrset(win[MENU_W], MENU_CLR);

	init_pair(2, fg_field, bg_field);
	wattrset(win[EDIT_W], EDIT_CLR);

	init_pair(3, fg_popup, bg_popup);
	wattrset(win[INFO_W], POPUP_CLR);

	return OK;
}

int
change_theme_popup(void)
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

	wattron(win, BORDER_CLR);
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, BORDER_CLR);
	wbkgd(win, POPUP_CLR);

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
get_extra_opt(void)
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

	wbkgd(win, MENU_CLR);

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
get_help(void)
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

	wattron(win, BORDER_CLR);
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, BORDER_CLR);
	wbkgd(win, POPUP_CLR);

	curs_set(0);

	wmove(win, line++, win_width / 3);
	waddstr(win, "YATE v1.1b");
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
open_file_popup(void)
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

	wattron(win, BORDER_CLR);
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, BORDER_CLR);
	wbkgd(win, POPUP_CLR);

	echo();

	mvwaddstr(win, 1, 1, " Enter file name: ");
	wrefresh(win);
	memset(content.file.name, '\0', FILENAME_MAX);
	scrollok(win, TRUE);
	idlok(win, TRUE);
	nonl();
	mvwgetstr(win, 1, 19, content.file.name);

	noecho();

	wclear(win);
	wrefresh(win);
	delwin(win);

	return OK;
}

int
save_file_popup(void)
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

	wattron(win, BORDER_CLR);
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, BORDER_CLR);
	wbkgd(win, POPUP_CLR);

	echo();

	mvwaddstr(win, 1, 1, " Enter file name: ");
	wrefresh(win);
	memset(content.file.name, '\0', FILENAME_MAX);
	mvwgetstr(win, 1, 19, content.file.name);

	noecho();

	wclear(win);
	wrefresh(win);
	delwin(win);

	return OK;
}
