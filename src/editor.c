/********************************************************************
 * PROGRAM: yate
 * FILE: editor.c
 * PURPOSE: main editor functionality
 * AUTHOR: 5aboteur <5aboteur@protonmail.com>
 *******************************************************************/

#include "editor.h"
#include "gui.h"
#include "movement.h"

/*
 * Function: enable_raw_mode
 * -------------------------
 * Description:
 *  Enables the "raw" mode for the terminal. By default most of the
 *  terminals start in canonical or "cooked" mode and your program can
 *  get the keyboard input only if user presses the 'Enter' button.
 *  "Raw" mode allows you to process each keypress as it comes in.
 */

void
enable_raw_mode(void)
{
	tcgetattr(STDIN_FILENO, &term_attr);

	struct termios raw = term_attr;

	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/*
 * Function: disable_raw_mode
 * --------------------------
 * Description:
 *  Disables the "raw" mode for the terminal.
 */

void
disable_raw_mode(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_attr);
}

/*
 * Function: open_file
 * -------------------
 * Description:
 *  Opens the file and puts all its parameters & content into the
 * 'content' structure.
 *
 * Arguments:
 *  'popup' - the flag that indicates wheter a popup window will be
 *            displayed or not.
 *
 * Asserts:
 *  'realloc' won`t return NULL.
 *
 * Notes:
 *  We don`t want to kill the program if something happened with
 *  the file that we tried to open and get its parameters. So, if
 *  an error occurred, its description appears in the status bar.
 */

void
open_file(int popup)
{
	if (popup) open_file_popup();

	FILE *fp = fopen(content.info.fname, "r");
	if (fp == NULL) {
		set_status(1, "open file '%s'", content.info.fname);
		return;
	}

	struct stat sb;

	if (lstat(content.info.fname, &sb) == ERR) {
		set_status(1, "get stat for '%s'", content.info.fname);
		return;
	}

	content.info.fsize = sb.st_size;
	content.info.fmode = sb.st_mode;

	// Cleanup the 'content' structure if it`s not empty
	if (content.nrows != 0) {
		free_content();
		init_content();
	}

	char *line = NULL;
	size_t sz = 0;
	ssize_t len;

	while ((len = getline(&line, &sz, fp)) != ERR) {
		while (len > 0 && (line[len - 1] == '\n' ||
			line[len - 1] == '\r')) {
			len--;
		}

		// Convert tabs to spaces
		for (int i = 0; i < len; ++i) {
			if (line[i] == '\t') {
				line = realloc(line, sizeof(char) *
					(len + YATE_TABSIZE + 1));

				assert(line != NULL);

				memmove(&line[i + YATE_TABSIZE], &line[i + 1],
					sizeof(char) * (len - i + 1));

				for (int j = 0; j < YATE_TABSIZE; ++j)
					line[i + j] = ' ';

				i += YATE_TABSIZE - 1;
				len += YATE_TABSIZE - 1;
			}
		}

		insert_row(content.nrows, line, len);
	}

	free(line);
	fclose(fp);

	wclear(win[INFO_W]);
	draw_window(INFO_W);

	set_status(0, "file successfully opened");
}

/*
 * Function: save_file
 * -------------------
 * Description:
 *  Saves the 'content' data into the file.
 *
 * Notes:
 *  We don`t want to kill the program if something happened with
 *  the file when we tried to store our data into it. So, if
 *  an error occurred, its description appears in the status bar.
 */

void
save_file(void)
{
	save_file_popup();

	FILE *fp = fopen(content.info.fname, "w");
	if (fp == NULL) {
		set_status(1, "open file '%s'", content.info.fname);
		return;
	}

	size_t buflen = 0;
	char *buf = rows_to_string(&buflen);

	size_t wsize = fwrite(buf, sizeof(char), buflen, fp);
	if (wsize != buflen) {
		set_status(1, "write to '%s'", content.info.fname);
		free(buf);
		fclose(fp);
		return;
	}

	content.info.fsize = buflen;

	free(buf);
	fclose(fp);

	wclear(win[INFO_W]);
	draw_window(INFO_W);

	set_status(0, "file successfully saved");
}

/*
 * Function: horiz_tab
 * ------------------------
 * Description:
 *  'Tab' (\t) key handler.
 *
 * Asserts:
 *  X and Y offsets can`t go beyond of the boundaries.
 *  'realloc' won`t return NULL.
 */

void
horiz_tab(void)
{
	int xoff = content.x_pos + content.x_off - DEFPOS_X;
	int yoff = content.y_pos + content.y_off - DEFPOS_Y;

	assert(xoff >= 0);
	assert(yoff >= 0);

	if (content.row == NULL)
		insert_row(yoff, "", 0);

	row_t *row = &content.row[yoff];
	int pos = (xoff > row->length) ? row->length : xoff;

	// Increase allocated memory chunk by editor tab size value
	row->chars = realloc(row->chars, sizeof(char) *
		(row->length + YATE_TABSIZE + 1));

	assert(row->chars != NULL);

	memmove(&row->chars[pos + YATE_TABSIZE], &row->chars[pos],
		sizeof(char) * (row->length - pos));

	row->length += YATE_TABSIZE;
	row->chars[row->length] = '\0';

	for (int i = 0; i < YATE_TABSIZE; ++i)
		row->chars[pos + i] = ' ';

	if (content.x_pos + YATE_TABSIZE < LIMIT_X)
		content.x_pos += YATE_TABSIZE ;
	else {
		content.x_off += YATE_TABSIZE - (LIMIT_X - content.x_pos);
		content.x_pos = LIMIT_X;
	}
}

/*
 * Function: next_line
 * -------------------
 * Description:
 *  'Enter' (\n) key handler.
 *
 * Asserts:
 *  X and Y offsets can`t go beyond of the boundaries.
 */

void
next_line(void)
{
	int xoff = content.x_pos + content.x_off - DEFPOS_X;
	int yoff = content.y_pos + content.y_off - DEFPOS_Y;

	assert(xoff >= 0);
	assert(yoff >= 0);

	if (xoff > 0) {
		row_t *row = &content.row[yoff];
		int pos = (xoff > row->length) ? row->length : xoff;

		/* Split the string into two parts and insert the second one
		   into the next line */
		insert_row(yoff + 1, &row->chars[pos], row->length - pos);

		// Redefine the pointer because of previous realloc
		row = &content.row[yoff];
		row->length = pos;
		row->chars[row->length] = '\0';
	} else
		insert_row(yoff, "", 0);

	content.x_pos = DEFPOS_X;
	content.x_off = 0;

	if (content.y_pos < LIMIT_Y)
		content.y_pos++;
	else
		content.y_off++;
}

/*
 * Function: insert_char
 * ---------------------
 * Description:
 *  Inserts a single char into the current row in the field.
 *
 * Arguments:
 *  'ch' - a character to be inserted.
 *
 * Asserts:
 *  X and Y offsets can`t go beyond of the boundaries.
 *  'realloc' won`t return NULL.
 */

void
insert_char(int ch)
{
	// We don`t want to handle control chars, so just skip them
	if (iscntrl(ch)) {
		flushinp();
		return;
	}

	if (content.info.encryption == TRUE)
		ch ^= 1;

	int xoff = content.x_pos + content.x_off - DEFPOS_X;
	int yoff = content.y_pos + content.y_off - DEFPOS_Y;

	assert(xoff >= 0);
	assert(yoff >= 0);

	if (content.row == NULL || yoff == content.nrows)
		insert_row(content.nrows, "", 0);

	row_t *row = &content.row[yoff];
	int pos = (xoff > row->length) ? row->length : xoff;

	// Reallocate the memory chunk for 'ch' and '\0'
	row->chars = realloc(row->chars, row->length + 2);

	assert(row->chars != NULL);

	// Split & move the second part of the row to the right
	memmove(&row->chars[pos + 1], &row->chars[pos],
		row->length - pos + 1);

	// And insert the character
	row->chars[pos] = ch;
	row->length++;

	move_right();

	set_status(0, "char '%c' inserted in [%d;%d]",
		ch, yoff + DEFPOS_Y, pos + DEFPOS_X);
}

/*
 * Function: remove_char
 * ---------------------
 * Description:
 *  'Delete' or 'Backspace' keys handler.
 *
 * Asserts:
 *  X and Y offsets can`t go beyond of the boundaries.
 *  'realloc' won`t return NULL.
 */

void
remove_char(void)
{
	if (content.row == NULL) return; // nothing to remove

	int xoff = content.x_pos + content.x_off - DEFPOS_X;
	int yoff = content.y_pos + content.y_off - DEFPOS_Y;

	assert(xoff >= 0);
	assert(yoff >= 0);

	row_t *row = &content.row[yoff];
	int pos = (xoff > row->length) ? row->length : xoff;

	if (xoff > 0) {
		char ch = row->chars[pos - 1];

		memmove(&row->chars[pos - 1], &row->chars[pos],
			row->length - pos);

		row->length--;

		if (row->length > 0) {
			row->chars = realloc(row->chars, row->length);
			assert(row->chars != NULL);
		}

		row->chars[row->length] = '\0';
		move_left();

		set_status(0, "char '%c' removed from [%d;%d]",
			ch, yoff + DEFPOS_Y, pos + DEFPOS_X);
	}
}

/*
 * Function: insert_row
 * --------------------
 * Description:
 *  Whole text in the screen is the array of text lines (rows) which
 *  located in the 'content' stucture. This function inserts a new row
 *  at the 'idx' position of array.
 *
 * Arguments:
 *  'idx' - index of the array where the row will be located;
 *  'line' - line of chars;
 *  'len' - length of the line.
 *
 * Asserts:
 *  'idx' is in the (0 <= idx <= nrows) range;
 *  'malloc' & 'realloc' won`t return NULL.
 */

void
insert_row(int idx, char *line, size_t len)
{
	assert(idx >= 0);
	assert(idx <= content.nrows);

	content.row = realloc(content.row,
		sizeof(row_t) * (content.nrows + 1));

	assert(content.row != NULL);

	// Shift the remaining part of the text down into 1 line
	memmove(&content.row[idx + 1], &content.row[idx],
		sizeof(row_t) * (content.nrows - idx));

	content.row[idx].length = len;
	content.row[idx].chars = malloc(sizeof(char) * (len + 1));

	assert(content.row[idx].chars != NULL);

	memcpy(content.row[idx].chars, line, len);

	content.row[idx].chars[len] = '\0';
	content.nrows++;
}

/*
 * Function: rows_to_string
 * ------------------------
 * Description:
 *  Before dump the editor`s content to the file, we need to transform
 *  rows from 'row_t' type into the char array.
 *
 * Arguments:
 *  'buflen' - output value that stores length of the result buffer.
 *
 * Asserts:
 *  'malloc' won`t return NULL.
 *
 * Returns:
 *  An array of rows in the text format.
 */

char *
rows_to_string(size_t *buflen)
{
	size_t total_len = 0;
	row_t *row = content.row;

	for (int i = 0; i < content.nrows; ++i)
		total_len += row[i].length + 1;

	*buflen = total_len;

	char *result = malloc(sizeof(char) * total_len);

	assert(result != NULL);

	char *pbuf = result;

	for (int i = 0; i < content.nrows; ++i, ++pbuf) {
		memcpy(pbuf, row[i].chars, row[i].length);
		pbuf += row[i].length; // jump to the next row
		*pbuf = '\n';
	}

	return result;
}

/*
 * Function: get_config
 * --------------------
 * Description:
 *  Gets CWD and append to it '.config' string.
 */

void
get_config(void)
{
	char cwd[PATH_MAX];

	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		set_status(1, "config path not defined");
		return;
	}

	snprintf(CONFIG_PATH, sizeof(CONFIG_PATH),
		"%s/%s", cwd, ".config");
}

/*
 * Function: load_config
 * ---------------------
 * Description:
 *  Loads the config file and parse its attributes.
 */

void
load_config(void)
{
	FILE *fp = fopen(CONFIG_PATH, "r");
	if (fp == NULL)	{
		set_status(1, "open config file");
		return;
	}

	char *line = NULL;
	size_t sz = 0;
	ssize_t len;
	char theme = '0';

	while ((len = getline(&line, &sz, fp)) != ERR) {
		if ((strlen(line) >= 7) && (strncmp(line, "theme", 5) == 0)) {
			theme = line[6];
			current_theme = theme;
			change_theme(0);
		}
	}

	free(line);
	fclose(fp);
}

/*
 * Function: save_config
 * ---------------------
 * Description:
 *  Saves current editor configuration to the config file.
 */

void
save_config(void)
{
	FILE *fp = fopen(CONFIG_PATH, "w");
	if (fp == NULL)	{
		set_status(1, "open config file");
		return;
	}

	fprintf(fp, "theme %c\n", current_theme);
	fclose(fp);
}

/*
 * Function: encrypt_text
 * ----------------------
 * Description:
 *  Enables or disables input encryption.
 */

void
encrypt_text(int y)
{
	content.info.encryption ^= y;
	wclear(win[INFO_W]);
	draw_window(INFO_W);
}

/*
 * Function: init_content
 * ----------------------
 * Description:
 *  Initializes editor`s content with default values.
 */

void
init_content(void)
{
	content.x_pos = DEFPOS_X;
	content.y_pos = DEFPOS_Y;
	content.x_off = 0;
	content.y_off = 0;
	content.nrows = 0;
	content.row = NULL;
	content.info.encryption = FALSE;
	content.info.status = 0;
	strcpy(content.info.error_descr, "...");
}

/*
 * Function: free_content
 * ----------------------
 * Description:
 *  Frees allocated memory for the rows.
 */

void
free_content(void)
{
	for (int i = 0; i < content.nrows; ++i)
		free(content.row[i].chars);
	free(content.row);
}

/*
 * Function: print_text
 * --------------------
 * Description:
 *  Prints the text into the editor`s edit field.
 */

void
print_text(void)
{
	// Save current cursor position to restore it after text dump
	int prev_x_pos = content.x_pos;
	int prev_y_pos = content.y_pos;

	content.x_pos = DEFPOS_X;
	content.y_pos = DEFPOS_Y;

	wmove(win[EDIT_W], content.y_pos, content.x_pos);

	int x, y;

	for (y = 0; (y < content.nrows) && (y < LIMIT_Y); ++y) {
		int xoff = content.x_off;
		int yoff = y + content.y_off;

		for (x = 0; (xoff < content.row[yoff].length) &&
			(x < LIMIT_X - 1); ++x, ++xoff) {
			waddch(win[EDIT_W], content.row[yoff].chars[xoff]);
			move_right();
		}

		// Put the '$' to show that current row has continuations
		if ((xoff != content.row[yoff].length) && (x == LIMIT_X - 1))
			waddch(win[EDIT_W], '$');
		if (content.x_off > 0)
			mvwaddch(win[EDIT_W], content.y_pos, DEFPOS_X, '$');

		content.x_pos = DEFPOS_X;
		content.y_pos++;
		wmove(win[EDIT_W], content.y_pos, content.x_pos);
	}

	content.x_pos = prev_x_pos;
	content.y_pos = prev_y_pos;
}

/*
 * Function: set_status
 * --------------------
 * Description:
 *  Prints the error message (if it) into the status bar.
 *
 * Arguments:
 *  'status' - editor state (0:ok, 1:err);
 *  'descr' - error description;
 *  '...' - list of arguments for 'descr'.
 */

void
set_status(int status, char *format, ...)
{
	content.info.status = status;

	va_list args;
	va_start(args, format);

	vsprintf(content.info.error_descr, format, args);
	wclear(win[INFO_W]);
	draw_window(INFO_W);

	va_end(args);
}
