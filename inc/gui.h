#ifndef __GUI_H__
#define __GUI_H__

#include "editor.h"

#define MENU_CLR	COLOR_PAIR(1)
#define EDIT_CLR	COLOR_PAIR(2)
#define POPUP_CLR	COLOR_PAIR(3)
#define BORDER_CLR	COLOR_PAIR(4)

void draw_edit(enum win_t wtype);
void draw_info(enum win_t wtype);
void draw_menu(enum win_t wtype);
void draw_window(enum win_t wtype);

int change_theme(void);
int change_theme_popup(void);

int get_extra_opt(void);
int get_help(void);

int open_file_popup(void);
int save_file_popup(void);

inline void __attribute__ ((always_inline))
update_gui(void)
{
	mvwprintw(win[INFO_W], 0, COLS / 2 - 6, " %3d : %3d ",
		content.y_pos, content.x_pos);

	for (int i = 0; i < NWINDOWS; ++i) {
		touchwin(win[i]);
		wnoutrefresh(win[i]);
	}

	wmove(win[EDIT_W], content.y_pos, content.x_pos);
	doupdate();
}

#endif
