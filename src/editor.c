#include "editor.h"
#include "gui.h"
#include "movement.h"

//char filename[FILENAME_MAX];

void
init_line(line_t *line)
{
	line->cur_size = 0;
	line->max_size = LINE_MAX;
	line->buf = calloc(LINE_MAX, sizeof(char));
}

void
expand_line(line_t *line)
{
	size_t new_size = line->max_size * 2;
	char *tmp = calloc(new_size, sizeof(char));

	strcpy(tmp, line->buf);
	free(line->buf);

	line->buf = tmp;
	line->max_size = new_size;
}

void
append_char(line_t *line, char ch)
{
	insert_char(line, ch, line->cur_size);
}

void
insert_char(line_t *line, char ch, size_t index)
{
	if (line->cur_size + 1 >= line->max_size)
		expand_line(line);

	for (size_t i = line->cur_size; i >= index; --i)
		line->buf[i + 1] = line->buf[i];

	line->buf[index] = ch;
	line->cur_size++;
}

void
remove_char(line_t *line, size_t index)
{
	for (size_t i = index; i < line->cur_size; ++i)
		line->buf[i] = line->buf[i + 1];

	line->buf[line->cur_size] = '\0';
	line->cur_size--;
}

void
horizontal_tab(void)
{
	content.buf_pos = (content.y_pos - 1) * COLS + content.x_pos;

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
	content.buf_pos = (content.y_pos - 1) * COLS + content.x_pos;

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

	content.buf_pos = (content.y_pos - 1) * COLS + content.x_pos;

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
/*
void
remove_char(void)
{
	move_left();
	waddch(win[EDIT_W], ' ');

	content.buf_pos = (content.y_pos - 1) * COLS + content.x_pos;

	if ((content.size < BUFSIZ || content.buf_pos < content.size) &&
		content.data[content.buf_pos]) {
		content.data[content.buf_pos] = ' ';
		content.size++;
	}
}
*/
void
print_text(void)
{
	content.x_pos = DEFPOS_X;
	content.y_pos = DEFPOS_Y;

	// Repaint window
	wclear(win[EDIT_W]);
	wattron(win[EDIT_W], BORDER_CLR);
	box(win[EDIT_W], ACS_VLINE, ACS_HLINE);
	wattroff(win[EDIT_W], BORDER_CLR);

	wmove(win[EDIT_W], content.y_pos, content.x_pos);

	// Dump text
	for (size_t i = 0; i < content.size; ++i) {
		if (content.data[i] == '\n') {
			content.x_pos = DEFPOS_X;
			content.y_pos++;
			wmove(win[EDIT_W], content.y_pos, content.x_pos);
		} else {
			waddch(win[EDIT_W], content.data[i]);
			move_right();
		}
	}

	content.x_pos = DEFPOS_X;
	content.y_pos = DEFPOS_Y;
}

int
open_file(bool from_arg)
{
	if (!from_arg && open_file_popup() == ERR) {
		perror("open_file_popup");
		return ERR;
	}

	FILE *fp = fopen(filename, "r");

	if (fp == NULL) {	// MOVE2 INFO BAR
		fprintf(stderr, "Error opening file '%s', skipped.", filename);
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

	while ((ch = fgetc(fp)) != EOF)
		content.data[i++] = ch;

	content.data[i] = '\0';

	content.size = i;
	fclose(fp);

	return OK;
}

int
save_file(void)
{
	if (save_file_popup() == ERR) {
		perror("save_file_popup");
		return ERR;
	}

	FILE *fp = fopen(filename, "w");

	if (fp == NULL) {	// MV2INFOBAR
		fprintf(stderr, "Error opening file '%s', skipped.", filename);
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
