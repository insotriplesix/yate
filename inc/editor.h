#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <ctype.h>
#include <limits.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define KEY_BS '\b'
#define KEY_CR '\r'
#define KEY_HT '\t'
#define KEY_NL '\n'

#define CTRL_E 5
#define CTRL_F 6
#define CTRL_G 7
#define CTRL_K 11
#define CTRL_O 15
#define CTRL_X 24
#define CTRL_Y 25

#define NWINDOWS 3

enum win_t { MENU_W, EDIT_W, INFO_W };

WINDOW *win[NWINDOWS];

extern char filename[FILENAME_MAX];

int change_theme();
int change_theme_popup();

int get_extra();
int get_help();

int open_file(char **buf, int *size, bool from_arg);
int open_file_popup();

int save_file(char *buf, int size);
int save_file_popup();

#endif
