#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define CONFIG_FILE "/home/saboteur/Programming/github/YATE/.config"

#define DEFPOS_X 1
#define DEFPOS_Y 1
#define NWINDOWS 3

WINDOW *win[NWINDOWS];

enum win_t { MENU_W, EDIT_W, INFO_W };

int encryption;
char current_theme;

struct termios term_attr;

typedef struct {
	char name[FILENAME_MAX];
	size_t size;
	unsigned long mode;
} filestat_t;

struct win_cont_t {
	int x_pos;
	int y_pos;
	int y_off;
	size_t buf_pos;
	size_t size;
	char *data;
	filestat_t file;
};

struct win_cont_t content;

void enable_raw_mode(void);
void disable_raw_mode(void);

void horizontal_tab(void);
void next_line(void);
void print_char(int ch);
void remove_char(void);

void print_text(void);

char **split_s(char *str, const char delim);

int open_file(void);
int open_file_ed(void);
int save_file(void);

#endif
