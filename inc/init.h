#ifndef __INIT_H__
#define __INIT_H__

#include <limits.h>
#include <ncurses.h>
#include <string.h>

int init_ncurses();
int init_colors();
int init_windows(WINDOW **win, int height, int width);

#endif
