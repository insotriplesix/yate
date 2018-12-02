/********************************************************************
 * PROGRAM: yate
 * FILE: gui.c
 * PURPOSE: editor gui
 * AUTHOR: 5aboteur <5aboteur@protonmail.com>
 *******************************************************************/

#include "gui.h"

/*
 * Function: update_gui
 * --------------------
 * Description:
 *  Updates program gui.
 */

void
update_gui(void)
{
	mvwprintw(win[INFO_W], 0, COLS / 2 - 16,
		" y: %3d, x: %3d, yo: %4d, xo: %4d ",
		content.y_pos, content.x_pos,
		content.y_off, content.x_off);

	wclear(win[EDIT_W]);
	draw_window(EDIT_W);

	for (int i = 0; i < NWINDOWS; ++i) {
		touchwin(win[i]);
		wnoutrefresh(win[i]);
	}

	print_text();

	wmove(win[EDIT_W], content.y_pos, content.x_pos);
	doupdate();
}

/*
 * Function: draw_window
 * ---------------------
 * Description:
 *  Draws a window.
 *
 * Arguments:
 *  'wtype' - specifies the window to draw (e.g. menu).
 */

void
draw_window(enum win_t wtype)
{
	wattron(win[wtype], BORDER_CLR);
	box(win[wtype], ACS_VLINE, ACS_HLINE);
	wattroff(win[wtype], BORDER_CLR);

	switch (wtype) {
		case MENU_W: draw_menu(wtype); break;
		case EDIT_W: draw_edit(wtype); break;
		case INFO_W: draw_info(wtype); break;
		default: break;
	}

	wrefresh(win[wtype]);
}

/*
 * Function: draw_edit
 * -------------------
 * Description:
 *  Draws the edit window.
 *
 * Arguments:
 *  'wtype' - type of a window (EDIT_W in this case).
 */

void
draw_edit(enum win_t wtype)
{
	wbkgd(win[wtype], EDIT_CLR);
}

/*
 * Function: draw_info
 * -------------------
 * Description:
 *  Draws the info window.
 *
 * Arguments:
 *  'wtype' - type of a window (INFO_W in this case).
 */

void
draw_info(enum win_t wtype)
{
	wbkgd(win[wtype], MENU_CLR);

	int offset = 4;

	char fname[FILENAME_MAX];
	info_t *info = &content.info;
	long namelen = strlen(info->fname);
	long errlen = strlen(info->error_descr);

	if (namelen > 16) {
		strncpy(fname, info->fname + (namelen - 16), sizeof(char) * 19);
		fname[0] = '.'; fname[1] = '.'; fname[2] = '.';
		fname[16] = '\0';
		mvwprintw(win[wtype], 2, offset, "FILE: %s", fname);
		offset += 26;
	} else {
		sprintf(fname, "%s", info->fname);
		mvwprintw(win[wtype], 2, offset, "FILE: %s", fname);
		offset += 10 + namelen;
	}

	mvwprintw(win[wtype], 2, offset, "SIZE: %lld b", info->fsize);
	offset += 15;
	mvwprintw(win[wtype], 2, offset, "MODE: %lo", info->fmode);
	offset += 16;
	mvwprintw(win[wtype], 2, offset, "ENCRYPT: %c",
		info->encryption ? 'Y' : 'N');
	offset += 14;

	if (COLS - 99 <= errlen) {
		mvwprintw(win[wtype], 2, offset, "STATUS: %s",
			((info->status == 0) ? "ok" : "err"));
	} else {
		mvwprintw(win[wtype], 2, offset, "STATUS: %s, %s",
			((info->status == 0) ? "ok" : "err"), info->error_descr);
	}

	mvwprintw(win[wtype], 2, COLS - 10, "v1.3f");

	wattron(win[wtype], BORDER_CLR);
}

/*
 * Function: draw_menu
 * -------------------
 * Description:
 *  Draws the menu window.
 *
 * Arguments:
 *  'wtype' - type of a window (MENU_W in this case).
 */

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

/*
 * Function: change_theme
 * ----------------------
 * Description:
 *  Changes current theme to another one that was received from the
 *  config file or selected by user.
 *
 * Arguments:
 *  'popup' - the flag that indicates wheter a popup window will be
 *            displayed or not.
 */

void
change_theme(int popup)
{
	int fg_field, bg_field,
		fg_menu, bg_menu,
		fg_popup, bg_popup;

	if (popup) current_theme = (char)change_theme_popup();

	switch (current_theme) {
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
			set_status(0, "...");
			return;
	}

	init_pair(1, fg_menu, bg_menu);
	wattrset(win[MENU_W], MENU_CLR);

	init_pair(2, fg_field, bg_field);
	wattrset(win[EDIT_W], EDIT_CLR);

	init_pair(3, fg_popup, bg_popup);
	wattrset(win[INFO_W], POPUP_CLR);

	if (popup) {
		set_status(0, "theme successfully changed to '%c'",
			current_theme);
	}
}

/*
 * Function: change_theme_popup
 * ----------------------------
 * Description:
 *  Draws 'themes' popup window.
 *
 * Asserts:
 *  'newpad' won`t return NULL.
 *
 * Returns:
 *  'choice' value that contains a theme number.
 */

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

	assert(win != NULL);

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

	flushinp();
	wclear(win);
	wrefresh(win);
	delwin(win);
	curs_set(1);

	return choice;
}

/*
 * Function: get_extra
 * -------------------
 * Description:
 *  Draws 'extra' popup window.
 *
 * Asserts:
 *  'newpad' won`t return NULL.
 */

void
get_extra(void)
{
	WINDOW *win;

	int win_height = 3;
	int win_width = 14;
	int line = 0;
	int offset_y = 2;
	int offset_x = 32;

	win = newpad(win_height, win_width);

	assert(win != NULL);

	wbkgd(win, MENU_CLR);
	curs_set(0);

	wmove(win, line++, 1);
	waddstr(win, "------------");
//	wmove(win, line++, 1);
//	waddstr(win, "^W - hexedit");
	wmove(win, line++, 1);
	waddstr(win, "^Y - livexor");
	wmove(win, line++, 1);
	waddstr(win, "^G - themez");

	prefresh(win, 0, 0,
		offset_y, offset_x,
		offset_y + win_height,
		offset_x + win_width);

	wgetch(win);

	flushinp();
	wclear(win);
	wrefresh(win);
	delwin(win);
	curs_set(1);
}

/*
 * Function: get_help
 * ------------------
 * Description:
 *  Draws 'help' popup window.
 *
 * Asserts:
 *  'newpad' won`t return NULL.
 */

void
get_help(void)
{
	WINDOW *win;

	int win_height = (LINES >= 14) ? 14 : 6;
	int win_width = (LINES >= 14) ? 30 : 34;
	int line = 1;
	int offset_y = LINES / 2;
	int offset_x = COLS / 2;

	win = newpad(win_height, win_width);

	assert(win != NULL);

	wattron(win, BORDER_CLR);
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, BORDER_CLR);
	wbkgd(win, POPUP_CLR);

	curs_set(0);

	if (LINES >= 14) {
		wmove(win, line++, win_width / 3);
		waddstr(win, "YATE v1.3f");
		wmove(win, line++, 1);
		waddstr(win, "");
		wmove(win, line++, 1);
		waddstr(win, "--- cmd --------------------");
		wmove(win, line++, 1);
		waddstr(win, " F4, ^O - open file");
		wmove(win, line++, 1);
		waddstr(win, " F5, ^S - save to file");
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
//		wmove(win, line++, 1);
//		waddstr(win, " ^W - show hex editor");
		wmove(win, line++, 1);
		waddstr(win, " ^Y - live encryption");
		wmove(win, line++, 1);
		waddstr(win, " ^G - change theme");
	} else {
		wmove(win, line++, 1);
		waddstr(win, " F4, ^O - open  | F5, ^S - save");
		wmove(win, line++, 1);
		waddstr(win, " F6, ^E - extra | F7, ^F - help");
		wmove(win, line++, 1);
		waddstr(win, " F8, ^X - exit  |");
		wmove(win, line++, 1);
		waddstr(win, " ^Y - encrypt   | ^G - themes");
	}

	prefresh(win, 0, 0,
		offset_y - win_height / 2,
		offset_x - win_width / 2,
		offset_y + win_height / 2,
		offset_x + win_width / 2);

	wgetch(win);

	flushinp();
	wclear(win);
	wrefresh(win);
	delwin(win);
	curs_set(1);
}

/*
 * Function: open_file_popup
 * -------------------------
 * Description:
 *  Draws popup window which is waits for the input.
 *
 * Asserts:
 *  'newwin' won`t return NULL.
 */

void
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

	assert(win != NULL);

	wattron(win, BORDER_CLR);
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, BORDER_CLR);
	wbkgd(win, POPUP_CLR);

	echo();

	mvwaddstr(win, 1, 1, " Enter file name: ");
	wrefresh(win);

	memset(content.info.fname, '\0', FILENAME_MAX);
	mvwgetstr(win, 1, 19, content.info.fname);

	flushinp();
	noecho();

	wclear(win);
	wrefresh(win);
	delwin(win);
}

/*
 * Function: save_file_popup
 * -------------------------
 * Description:
 *  Draws popup window which is waits for the input.
 *
 * Asserts:
 *  'newwin' won`t return NULL.
 */

void
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

	assert(win != NULL);

	wattron(win, BORDER_CLR);
	box(win, ACS_VLINE, ACS_HLINE);
	wattroff(win, BORDER_CLR);
	wbkgd(win, POPUP_CLR);

	echo();

	mvwaddstr(win, 1, 1, " Enter file name: ");
	wrefresh(win);

	memset(content.info.fname, '\0', FILENAME_MAX);
	mvwgetstr(win, 1, 19, content.info.fname);

	flushinp();
	noecho();

	wclear(win);
	wrefresh(win);
	delwin(win);
}
