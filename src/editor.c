#include "editor.h"
#include "gui.h"
#include "movement.h"

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

void
disable_raw_mode(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &term_attr);
}

int
insert_row(int idx, char *line, size_t len)
{
	if (idx < 0 || idx > content.nrows)
		return ERR;

	content.row = realloc(content.row, sizeof(row_t) * (content.nrows + 1));
	memmove(&content.row[idx + 1], &content.row[idx], sizeof(row_t) * (content.nrows - idx));

	if (content.row == NULL)
		return ERR;

	content.row[idx].length = len;
	content.row[idx].chars = malloc(sizeof(char) * (len + 1));

	memcpy(content.row[idx].chars, line, len);

	content.row[idx].chars[len] = '\0';
	content.nrows++;

	return OK;
}

char *
rows_to_string(size_t *buflen)
{
	size_t total_sz = 0;
	row_t *row = content.row;

	for (int i = 0; i < content.nrows; ++i) {
		total_sz += row[i].length + 1;
	}

	*buflen = total_sz;

	char *result = malloc(sizeof(char) * total_sz);
	if (result == NULL) {
		fprintf(stderr, "Not enough memory!\n");
		return NULL;
	}

	char *pbuf = result;

	for (int i = 0; i < content.nrows; ++i, ++pbuf) {
		memcpy(pbuf, row[i].chars, row[i].length);
		pbuf += row[i].length;
		*pbuf = '\n';
	}

	return result;
}

void
init_content(void)
{
	content.x_pos = DEFPOS_X;
	content.y_pos = DEFPOS_Y;
	content.x_off = 0;
	content.y_off = 0;
	content.nrows = 0;
	content.row = NULL;
	encryption = FALSE;
}

void
free_content(void)
{
	for (int i = 0; i < content.nrows; ++i)
		free(content.row[i].chars);
	free(content.row);
}

void
horizontal_tab(void)
{
/*	content.buf_pos = (content.y_pos - 1) * COLS +
		(content.y_off * COLS) + (content.x_pos - 1);

	if ((content.size < BUFSIZ || content.buf_pos < content.size) &&
		content.data[content.buf_pos]) {
		content.data[content.buf_pos] = '\t';
		content.size++;
	}

	for (int i = 0; i < 4; ++i) move_right();
*/}

int
next_line(void)
{
	int xoff = content.x_pos + content.x_off - DEFPOS_X;
	int yoff = content.y_pos + content.y_off - DEFPOS_Y;

	int rc = 0;

	if (xoff == 0) {
		rc = insert_row(yoff, "", 0);
		if (rc == ERR) return rc;
	} else {
		row_t *row = &content.row[yoff];
		int pos;

		if (xoff < 0 || xoff > row->length)
			pos = row->length;
		else
			pos = xoff;

		rc = insert_row(yoff + 1, &row->chars[pos], row->length - pos);
		if (rc == ERR) return rc;

		row = &content.row[yoff];
		row->length = xoff;
		row->chars[row->length] = '\0';
	}

	content.x_pos = DEFPOS_X;
	content.x_off = 0;

	if (content.y_pos < LIMIT_Y)
		content.y_pos++;
	else
		content.y_off++;

	return OK;
/*	content.buf_pos = (content.y_pos - 1) * COLS +
		(content.y_off * COLS) + (content.x_pos - 1);

	if ((content.size < BUFSIZ || content.buf_pos < content.size) &&
		content.data[content.buf_pos]) {
		content.data[content.buf_pos] = '\n';
		content.size++;
	}

	content.x_pos = DEFPOS_X;
	content.y_pos++;
*/}

int
insert_char(int ch)
{
	if (iscntrl(ch))
		return OK;

	if (encryption == TRUE)
		ch ^= 1;

	int xoff = content.x_pos + content.x_off - DEFPOS_X;
	int yoff = content.y_pos + content.y_off - DEFPOS_Y;

	if ((content.row == NULL) && (insert_row(content.nrows, "", 0) == ERR)) {
		return ERR;
	}

	row_t *row = &content.row[yoff];
	int pos;

	if (xoff < 0 || xoff > row->length)
		pos = row->length;
	else
		pos = xoff;

	row->chars = realloc(row->chars, row->length + 2);
	if (row->chars == NULL) {
		fprintf(stderr, "Not enough memory!\n");
		return ERR;
	}

	memmove(&row->chars[pos + 1], &row->chars[pos], row->length - pos + 1);

	row->length++;
	row->chars[pos] = ch;

	move_right();

	return OK;
/*
	content.buf_pos = (content.y_pos - 1) * COLS +
		(content.y_off * COLS) + (content.x_pos - 1);

	if ((content.size < BUFSIZ || content.buf_pos < content.size)) {// &&
//		content.data[content.buf_pos]) {
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
//	mvwprintw(win[EDIT_W], 5, 5, "%d, %s\n", content.buf_pos, content.data);
*/}

int
remove_char(void)
{
	move_left();

	int xoff = content.x_pos + content.x_off - DEFPOS_X;
	int yoff = content.y_pos + content.y_off - DEFPOS_Y;
	row_t *row = &content.row[yoff];
	int pos;

	if (xoff < 0 || xoff > row->length)
		pos = row->length;
	else
		pos = xoff;

//	if (row->length == 1) {
//		row->chars[0] = ' ';
//		row->chars[1] = '\0';
//	} else {
	if (row->length) {
		memmove(&row->chars[pos], &row->chars[pos + 1], row->length - pos);
		row->length--;

		if (row->length) {
			row->chars = realloc(row->chars, row->length);
			if (row->chars == NULL) {
				fprintf(stderr, "Not enough memory!\n");
				return ERR;
			}
		}
		row->chars[row->length] = '\0';
	}

	return OK;


/*	move_left();
	waddch(win[EDIT_W], ' ');

	content.buf_pos = (content.y_pos - 1) * COLS +
		(content.y_off * COLS) + (content.x_pos - 1);

	if ((content.size < BUFSIZ || content.buf_pos < content.size) &&
		content.data[content.buf_pos]) {
		content.data[content.buf_pos] = ' ';
		content.size++;
	}
*/}

void
print_text(void)
{
	int prev_x_pos = content.x_pos;
	int prev_y_pos = content.y_pos;

	content.x_pos = DEFPOS_X;
	content.y_pos = DEFPOS_Y;

	// Repaint window
//	wclear(win[EDIT_W]);
//	wattron(win[EDIT_W], BORDER_CLR);
//	box(win[EDIT_W], ACS_VLINE, ACS_HLINE);
//	wattroff(win[EDIT_W], BORDER_CLR);

	wmove(win[EDIT_W], content.y_pos, content.x_pos);

	int yoff, xoff, col;

	for (yoff = 0; (yoff < content.nrows) && (yoff < LINES - 10); ++yoff) {
//		mvwprintw(win[EDIT_W], content.y_pos++, content.x_pos,
//			"%s", &content.row[i + content.y_off].chars[content.x_off]);
		int row_offset = yoff + content.y_off;
		for (col = 0, xoff = content.x_off; (xoff < content.row[row_offset].length) &&
			(col < COLS - 3); ++xoff, ++col) {
			waddch(win[EDIT_W], content.row[row_offset].chars[xoff]);
			move_right();
		}

		if ((xoff != content.row[row_offset].length) && (col == COLS - 3))
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

int
open_file(void)
{
	FILE *fp = fopen(content.file.name, "r");

	if (fp == NULL) {	// MOVE2 INFO BAR
		fprintf(stderr, "Error opening file '%s', skipped.",
			content.file.name);
		return OK;
	}

	struct stat sb;

	// MOVE2 INFO BAR
	if (lstat(content.file.name, &sb) == ERR) {
		perror("lstat");
		return OK;
	}

	content.file.size = sb.st_size;
	content.file.mode = sb.st_mode;

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
/*
		for (int i = 0; i < len; ++i) {
			if (line[i] == '\t') {
				line = realloc(line, sizeof(char) * (len + YATE_TABSIZE - 1));
				memmove(&line[i + YATE_TABSIZE - 1], &line[i], sizeof(char) * (len - i));
				for (int j = i; j < YATE_TABSIZE - 1; ++j) {
					line[j] = ' ';
				}

				len += YATE_TABSIZE - 1;
			}

		}
*/
		if (insert_row(content.nrows, line, len) == ERR)
			return ERR;
	}

	free(line);
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
	FILE *fp = fopen(content.file.name, "w");

	if (fp == NULL) {	// MV2INFOBAR
		fprintf(stderr, "Error opening file '%s', skipped.",
			content.file.name);
		return OK;
	}

	size_t buflen = 0;
	char *buf = rows_to_string(&buflen);
	if (buf == NULL) {
		return ERR;
	}

	size_t wsize = fwrite(buf, sizeof(char), buflen, fp);
	if (wsize != buflen) {
		fprintf(stderr, "Error write to file.\n");

		free(buf);
		fclose(fp);

		return ERR;
	}

//	mvwprintw(win[EDIT_W], 20, 1, "%s", content.data);

	content.file.size = buflen;

	free(buf);
	fclose(fp);

	wclear(win[INFO_W]);
	draw_window(INFO_W);

	return OK;
//	size_t len = (content.size < BUFSIZ) ? BUFSIZ : content.size;

//	for (size_t i = 0; i < len; ++i) {
//		if (content.data[i] != ' ' && isalpha(content.data[i]))
//			len = i;
//	}
//	printf("%ld", content.size);
//	for (size_t i = 0; i < content.size; ++i)
//		fputc(content.data[i], fp);

//	fputc('\0', fp);

//	fputs(content.data, fp);
}

int
save_file_ed(void)
{
	if (save_file_popup() == ERR) {
		perror("save_file_popup");
		return ERR;
	}

	return save_file();
}
