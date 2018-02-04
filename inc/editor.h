#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <ctype.h>
#include <limits.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NWINDOWS 3

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

extern char filename[NAME_MAX];

int change_theme(WINDOW **win, int height, int width);
int change_theme_popup(int height, int width);

int get_help(int height, int width);
int help_popup(int height, int width);

void open_file(char *buf, int *size, int height, int width);
char *open_file_popup(int height, int width);

void save_file(char *buf, int size, int height, int width);
char *save_file_popup(int height, int width);

#endif
