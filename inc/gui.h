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

int change_theme(char theme);
int change_theme_ed(void);
int change_theme_popup(void);

int get_extra_opt(void);
int get_help(void);

int open_file_popup(void);
int save_file_popup(void);

void update_gui(void);

#endif
