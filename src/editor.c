#include "editor.h"
#include "gui.h"
#include "movement.h"

void
horizontal_tab(void)
{
	content.buf_pos = (content.y_pos - 1) * COLS +
		(content.y_off * COLS) + content.x_pos;

	if ((content.size < BUFSIZ || content.buf_pos < content.size) &&
		content.data[content.buf_pos]) {
		content.data[content.buf_pos] = '\t';
		content.size++;
	}

	for (int i = 0; i < 4; ++i) move_right();
}

void
next_line(void)
{
	content.buf_pos = (content.y_pos - 1) * COLS +
		(content.y_off * COLS) + content.x_pos;

	if ((content.size < BUFSIZ || content.buf_pos < content.size) &&
		content.data[content.buf_pos]) {
		content.data[content.buf_pos] = '\n';
		content.size++;
	}

	content.x_pos = DEFPOS_X;
	content.y_pos++;
}

void
print_char(int ch)
{
	if (iscntrl(ch))
		return;

	if (encryption == TRUE)
		ch ^= 1;

	content.buf_pos = (content.y_pos - 1) * COLS +
		(content.y_off * COLS) + content.x_pos;

	if ((content.size < BUFSIZ || content.buf_pos < content.size) &&
		content.data[content.buf_pos]) {
		content.data[content.buf_pos] = (char) ch;
		content.size++;
	}

	if (content.x_pos + 1 < COLS - 1) {
		waddch(win[EDIT_W], ch);
		move_right();
	} else {
		// Deal with eol
		waddch(win[EDIT_W], ch);
		content.x_pos = DEFPOS_X;
		content.y_pos++;
	}
}

void
remove_char(void)
{
	move_left();
	waddch(win[EDIT_W], ' ');

	content.buf_pos = (content.y_pos - 1) * COLS +
		(content.y_off * COLS) + content.x_pos;

	if ((content.size < BUFSIZ || content.buf_pos < content.size) &&
		content.data[content.buf_pos]) {
		content.data[content.buf_pos] = ' ';
		content.size++;
	}
}

void
print_text(void)
{
	int prev_x_pos = content.x_pos;
	int prev_y_pos = content.y_pos;

	content.x_pos = DEFPOS_X;
	content.y_pos = DEFPOS_Y;

	// Repaint window
	wclear(win[EDIT_W]);
	wattron(win[EDIT_W], BORDER_CLR);
	box(win[EDIT_W], ACS_VLINE, ACS_HLINE);
	wattroff(win[EDIT_W], BORDER_CLR);

	wmove(win[EDIT_W], content.y_pos, content.x_pos);

	size_t edit_win_size = (LINES - 9) * (COLS - 2);
	size_t print_size = (content.size < edit_win_size)
		? content.size : edit_win_size;

	int nl_count = 0;
	int offset = content.y_off * COLS;

	// Dump text
	for (size_t i = 0; i < print_size && nl_count < LINES - 10; ++i) {
		if (content.data[i + offset] == '\n') {
			content.x_pos = DEFPOS_X;
			content.y_pos++;
			wmove(win[EDIT_W], content.y_pos, content.x_pos);
			nl_count++;
		} else {
			waddch(win[EDIT_W], content.data[i + offset]);
			move_right();
		}
	}

//	content.x_pos = DEFPOS_X;
//	content.y_pos = DEFPOS_Y;
	content.x_pos = prev_x_pos;
	content.y_pos = prev_y_pos;
}


char **
split_s(char *str, const char delim)
{
	char **result = NULL;
	char *tmp = str;
	char *last_delim = NULL;
	size_t count = 0;

	char d[2] = { delim, '\0' };

	while (*tmp) {
		if (delim == *tmp) {
			count++;
			last_delim = tmp;
		}

		tmp++;
	}

	count += last_delim < (str + strlen(str) - 1);
	count++;

	result = malloc(sizeof(char *) * count);

	if (result) {
		size_t idx = 0;
		char *tok = strtok(str, d);

		while (tok) {
			assert(idx < count);
			*(result + idx++) = strdup(tok);
			tok = strtok(0, d);
		}

		assert (idx == count - 1);
		*(result + idx) = 0;
	}

	return result;
}

int
open_file(void)
{
	FILE *fp = fopen(content.file.name, "r");

	if (fp == NULL) {	// MOVE2 INFO BAR
		fprintf(stderr, "Error opening file '%s', skipped.",
			content.file.name);
		return OK;
	}

	// Get the file length
	fseek(fp, 0L, SEEK_END);
	long len = ftell(fp);
	rewind(fp);

	if (len > BUFSIZ && len > (long)content.size) {
		// Allocate more space for the buffer
		char *tmp = realloc(content.data, sizeof(char) * len + 1);

		if (tmp == NULL) {
			perror("realloc");
			fclose(fp);
			return ERR;
		}

		content.data = tmp;
	}

	char ch;
	int i = 0;

	struct stat sb;

	// MOVE2 INFO BAR
	if (lstat(content.file.name, &sb) == ERR) {
		perror("lstat");
		return OK;
	}

	content.file.size = sb.st_size;
	content.file.mode = sb.st_mode;

	while ((ch = fgetc(fp)) != EOF)
		content.data[i++] = ch;

	content.data[i] = '\0';

	content.size = i;
	fclose(fp);

	wclear(win[INFO_W]);
	draw_window(INFO_W);

	return OK;
}

int
open_file_ed(void)
{
	if (open_file_popup() == ERR) {
		perror("open_file_popup");
		return ERR;
	}

	return open_file();
}

int
save_file(void)
{
	if (save_file_popup() == ERR) {
		perror("save_file_popup");
		return ERR;
	}

	FILE *fp = fopen(content.file.name, "w");

	if (fp == NULL) {	// MV2INFOBAR
		fprintf(stderr, "Error opening file '%s', skipped.",
			content.file.name);
		return OK;
	}

	mvwprintw(win[EDIT_W], 20, 1, "%s", content.data);

//	size_t len = (content.size < BUFSIZ) ? BUFSIZ : content.size;

//	for (size_t i = 0; i < len; ++i) {
//		if (content.data[i] != ' ' && isalpha(content.data[i]))
//			len = i;
//	}

	for (size_t i = 0; i < content.size; ++i)
		fputc(content.data[i], fp);

	fputc('\0', fp);
	fclose(fp);

	return OK;
}
