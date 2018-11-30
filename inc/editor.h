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

#define DEFPOS_X 1
#define DEFPOS_Y 1

#define LIMIT_X (COLS - 2)
#define LIMIT_Y (LINES - 10)

#define NWINDOWS 3
#define YATE_TABSIZE 4

char CONFIG_PATH[PATH_MAX + FILENAME_MAX + 1];

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

typedef struct {
	int length;
	char *chars;
} row_t;

struct win_cont_t {
	int x_pos;
	int y_pos;
	int x_off;
	int y_off;
	int nrows;
	row_t *row;
	filestat_t file;
};

struct win_cont_t content;

void enable_raw_mode(void);
void disable_raw_mode(void);

int insert_row(int idx, char *line, size_t len);
char *rows_to_string(size_t *buflen);

void init_content(void);
void free_content(void);

void horizontal_tab(void);
int next_line(void);
int insert_char(int ch);
int remove_char(void);

void print_text(void);

int open_file(void);
int open_file_ed(void);
int save_file(void);
int save_file_ed(void);

#endif
