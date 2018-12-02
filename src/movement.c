/********************************************************************
 * PROGRAM: yate
 * FILE: movement.c
 * PURPOSE: arrow keys, page keys, home and end keys handlers
 * AUTHOR: 5aboteur <5aboteur@protonmail.com>
 *******************************************************************/

#include "movement.h"

/*
 * Function: move_left
 * -------------------
 * Description:
 *  Moves cursor on one character to the left.
 *
 * Asserts:
 *  Y-axis cursor offset can`t be less than zero.
 */

void
move_left(void)
{
	if (content.row == NULL) return;

	int yoff = content.y_pos + content.y_off;

	if (content.x_pos - 1 > 0) {
		content.x_pos--;
	} else if (content.x_off > 0) {
		content.x_off--;
	} else if (yoff != DEFPOS_Y) {
		assert(yoff - 2 >= 0);
		int prowlen = content.row[yoff - 2].length;
		if (prowlen < LIMIT_X) {
			content.x_pos = prowlen + 1;
		} else {
			content.x_pos = LIMIT_X;
			content.x_off = prowlen + 1 - LIMIT_X;
		}

		if (content.y_pos - 1 > 0)
			content.y_pos--;
		else if (content.y_off > 0)
			content.y_off--;
	}
}

/*
 * Function: move_right
 * --------------------
 * Description:
 *  Moves cursor on one character to the right.
 */

void
move_right(void)
{
	if (content.row == NULL) return;

	int xoff = content.x_pos + content.x_off;
	int yoff = content.y_pos + content.y_off;
	int rowlen = content.row[yoff - 1].length;

	if ((content.x_pos < LIMIT_X) && (xoff < rowlen + 1)) {
		content.x_pos++;
	} else if (xoff < rowlen + 1) {
		content.x_off++;
	} else if (yoff != content.nrows) {
		content.x_pos = DEFPOS_X;
		content.x_off = 0;

		if (content.y_pos < LIMIT_Y)
			content.y_pos++;
		else if (yoff < content.nrows)
			content.y_off++;
	}
}

/*
 * Function: move_up
 * -----------------
 * Description:
 *  Moves cursor up to the end of the previous row.
 */

void
move_up(void)
{
	if (content.row == NULL) return;

	int yoff = content.y_pos + content.y_off;

	if (content.y_pos - 1 > 0)
		content.y_pos--;
	else if (content.y_off > 0)
		content.y_off--;

	// If true then we reached the top
	if (yoff - 2 < 0) return;

	int prowlen = content.row[yoff - 2].length;

	if (prowlen < LIMIT_X) {
		content.x_pos = prowlen + 1;
		content.x_off = 0;
	} else {
		content.x_pos = LIMIT_X;
		content.x_off = prowlen + 1 - LIMIT_X;
	}
}

/*
 * Function: move_down
 * -------------------
 * Description:
 *  Moves cursor down to the end of the next row.
 */

void
move_down(void)
{
	if (content.row == NULL) return;

	int yoff = content.y_pos + content.y_off;

	if (content.y_pos < LIMIT_Y && yoff < content.nrows)
		content.y_pos++;
	else if ((content.y_pos < content.nrows) && (yoff < content.nrows))
		content.y_off++;

	// If true then we reached the bottom
	if (yoff >= content.nrows) return;

	int nrowlen = content.row[yoff].length;

	if (nrowlen < LIMIT_X) {
		content.x_pos = nrowlen + 1;
		content.x_off = 0;
	} else {
		content.x_pos = LIMIT_X;
		content.x_off = nrowlen + 1 - LIMIT_X;
	}
}

/*
 * Function: move_home
 * -------------------
 * Description:
 *  Moves cursor on 'COLS - 2' characters to the left.
 */

void
move_home(void)
{
	content.x_pos = DEFPOS_X;
}

/*
 * Function: move_end
 * ------------------
 * Description:
 *  Moves cursor on 'COLS - 2' characters to the right.
 */

void
move_end(void)
{
	if (content.row == NULL) return;
	int rowlen = content.row[content.y_pos + content.y_off - 1].length;
	content.x_pos = (rowlen > LIMIT_X) ? LIMIT_X : rowlen + 1;
}

/*
 * Function: move_npage
 * --------------------
 * Description:
 *  Moves cursor down to the last row on the screen. Also skip text
 *  up to the next page.
 */

void
move_npage(void)
{
	int nrows = content.nrows;
	int yoff = content.y_off;

	if (content.y_pos == LIMIT_Y) {
		content.y_off += ((LIMIT_Y*2 + yoff) > nrows)
				? nrows - (LIMIT_Y + yoff)
				: LIMIT_Y;
	} else {
		content.y_pos = (nrows > 0)
			? (nrows < LIMIT_Y ? nrows : LIMIT_Y)
			: DEFPOS_Y;
	}

	content.x_pos = DEFPOS_X;
	content.x_off = 0;
}

/*
 * Function: move_ppage
 * --------------------
 * Description:
 *  Moves cursor up to the first row on the screen. Also skip text
 *  up to the previous page.
 */

void
move_ppage(void)
{
	int yoff = content.y_off;

	if (content.y_pos == DEFPOS_Y) {
		content.y_off -= ((yoff - LIMIT_Y*2) < DEFPOS_Y)
			? ((yoff - LIMIT_Y >= 0) ? LIMIT_Y : yoff)
			: LIMIT_Y;
	} else {
		content.y_pos = DEFPOS_Y;
	}

	content.x_pos = DEFPOS_X;
	content.x_off = 0;
}
