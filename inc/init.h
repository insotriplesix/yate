/********************************************************************
 * PROGRAM: yate
 * FILE: init.h
 * PURPOSE: editor initialization functions
 * AUTHOR: 5aboteur <5aboteur@protonmail.com>
 *******************************************************************/

#ifndef __INIT_H__
#define __INIT_H__

#include "editor.h"

void initialize(int argc, char *argv[]);
void finalize(void);

int init_colors(void);
void init_gui(void);
int init_ncurses(void);
void init_windows(void);

#endif
