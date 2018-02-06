#ifndef __MOVEMENT_H__
#define __MOVEMENT_H__

#include <ncurses.h>

inline void __attribute__ ((always_inline))
move_left(int *y, int *x, int border)
{
	if (*x - 1 > border) wmove(win[EDIT_W], *y, --(*x));
}

inline void __attribute__ ((always_inline))
move_right(int *y, int *x, int border)
{
	if (*x + 1 < border) wmove(win[EDIT_W], *y, ++(*x));
}

inline void __attribute__ ((always_inline))
move_up(int *y, int *x, int border)
{
	if (*y - 1 > border) wmove(win[EDIT_W], --(*y), *x);
}

inline void __attribute__ ((always_inline))
move_down(int *y, int *x, int border)
{
	if (*y + 1 < border) wmove(win[EDIT_W], ++(*y), *x);
}

#endif
