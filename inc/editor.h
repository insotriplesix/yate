/********************************************************************
 * PROGRAM: yate
 * FILE: editor.h
 * PURPOSE: main editor functionality
 * AUTHOR: 5aboteur <5aboteur@protonmail.com>
 *******************************************************************/

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

/*
 * This implementation has 3 windows: menu, edit, info.
 *
 * +------------------+
 * | (1) Menu window  |
 * +------------------+
 * |                  |
 * | (2) Edit window  |
 * |                  |
 * +------------------+
 * | (3) Info window  |
 * +------------------+
 *
 * Menu window contains the set of F(k) keys that you can use and
 * get the editor functionality. For example, if you press the 'Help'
 * key, you`ll get the full list of control and function keys that
 * you can use in the program. In the edit field you can read the
 * text and change it if you want. Info window displays the current
 * state of the file (e.g.: name, mode, cursor offset, errors).
 */

#define NWINDOWS 3

/*
 * Each window framed, then default relative position of text will be
 * (y:1,x:1). X-axis limit: number of columns - 2 (left border and the
 * right one). Y-axis limit: number of lines - 10 (3 of menu window,
 * 5 of info, plus top & bottom borders).
 */

#define DEFPOS_X 1
#define DEFPOS_Y 1
#define LIMIT_X (COLS - 2)
#define LIMIT_Y (LINES - 10)

#define ERRDESCR_MAX 64
#define DEFAULT_FNAME "yate_untitled.txt"

/* Stores a path to the .config file */
char CONFIG_PATH[PATH_MAX + FILENAME_MAX + 1];

/* Editor windows */
WINDOW *win[NWINDOWS];

/* Window types */
enum win_t { MENU_W, EDIT_W, INFO_W };

/* Current theme used in the editor */
char current_theme;

/* Stores the default terminal attributes */
struct termios term_attr;

typedef struct {
	char fname[FILENAME_MAX];
	size_t fsize;
	unsigned long fmode;
	int encryption; // if 1 then every input char will be XORed with 1
	int status; // editor state 0:ok, 1:err
	char error_descr[ERRDESCR_MAX];
} info_t;

typedef struct {
	int length;
	char *chars;
} row_t;

/* The main structure that contains all necessary editor data */
struct win_cont_t {
	int x_pos;   // X-axis cursor position
	int y_pos;   // Y-axis cursor position
	int x_off;   // current row offset
	int y_off;   // rows array offset
	int nrows;   // number of rows
	row_t *row;  // rows array
	info_t info; // used for info window
};

struct win_cont_t content;

/* Terminal modes */
void enable_raw_mode(void);
void disable_raw_mode(void);

/* File actions */
void open_file(int popup);
void save_file(void);

/* Character manipulations */
void horiz_tab(void);
void next_line(void);
void insert_char(int ch);
void remove_char(void);

/* Rows manipulations */
void insert_row(int idx, char *line, size_t len);
char *rows_to_string(size_t *buflen);

/* Other */
void get_config(void);
void load_config(void);
void save_config(void);

void encrypt_text(int y);

void init_content(void);
void free_content(void);

void print_text(void);
void set_status(int status, char *format, ...);

#endif
