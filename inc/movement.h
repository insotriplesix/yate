#ifndef __MOVEMENT_H__
#define __MOVEMENT_H__

#include "editor.h"

inline void
__attribute__ ((always_inline))
move_left()
{
	if (content.x_pos - 1 > 0) content.x_pos--;
	wmove(win[EDIT_W], content.y_pos, content.x_pos);
}

inline void
__attribute__ ((always_inline))
move_right()
{
	if (content.x_pos + 1 < COLS - 1) content.x_pos++;
	wmove(win[EDIT_W], content.y_pos, content.x_pos);
}

inline void
__attribute__ ((always_inline))
move_up()
{
	if (content.y_pos - 1 > 0) content.y_pos--;
	wmove(win[EDIT_W], content.y_pos, content.x_pos);
}

inline void
__attribute__ ((always_inline))
move_down()
{
	if (content.y_pos + 1 < LINES - 9) content.y_pos++;
	wmove(win[EDIT_W], content.y_pos, content.x_pos);
}

#endif
