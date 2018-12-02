/********************************************************************
 * PROGRAM: yate
 * FILE: gui.h
 * PURPOSE: editor gui
 * AUTHOR: 5aboteur <5aboteur@protonmail.com>
 *******************************************************************/

#ifndef __GUI_H__
#define __GUI_H__

#include "editor.h"

#define MENU_CLR COLOR_PAIR(1)
#define EDIT_CLR COLOR_PAIR(2)
#define POPUP_CLR COLOR_PAIR(3)
#define BORDER_CLR COLOR_PAIR(4)

void update_gui(void);

void draw_window(enum win_t wtype);
void draw_edit(enum win_t wtype);
void draw_info(enum win_t wtype);
void draw_menu(enum win_t wtype);

void change_theme(int popup);
int change_theme_popup(void);

void get_extra(void);
void get_help(void);

void open_file_popup(void);
void save_file_popup(void);

#endif
