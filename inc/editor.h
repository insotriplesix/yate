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

#define DEFPOS_X 1
#define DEFPOS_Y 1
#define NWINDOWS 3

WINDOW *win[NWINDOWS];

enum win_t { MENU_W, EDIT_W, INFO_W };
extern char filename[FILENAME_MAX];

int encryption;

struct win_cont_t {
	int x_pos;
	int y_pos;
	size_t size;
	char *data;
};

struct win_cont_t content;

void horizontal_tab(void);
void next_line(void);
void print_char(int ch);
void remove_char(void);

void print_text(void);

int open_file(bool from_arg);
int save_file();

#endif
