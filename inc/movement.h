#ifndef __MOVEMENT_H__
#define __MOVEMENT_H__

#include <ncurses.h>

inline void __attribute__ ((always_inline))
move_left(WINDOW *win, int *y, int *x, int border)
{
	if (*x - 1 > border) wmove(win, *y, --(*x));
}

inline void __attribute__ ((always_inline))
move_right(WINDOW *win, int *y, int *x, int border)
{
	if (*x + 1 < border) wmove(win, *y, ++(*x));
}

inline void __attribute__ ((always_inline))
move_up(WINDOW *win, int *y, int *x, int border)
{
	if (*y - 1 > border) wmove(win, --(*y), *x);
}

inline void __attribute__ ((always_inline))
move_down(WINDOW *win, int *y, int *x, int border)
{
	if (*y + 1 < border) wmove(win, ++(*y), *x);
}

#endif
