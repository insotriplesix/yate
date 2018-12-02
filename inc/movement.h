/********************************************************************
 * PROGRAM: yate
 * FILE: movement.h
 * PURPOSE: arrow keys, page keys, home and end keys handlers
 * AUTHOR: 5aboteur <5aboteur@protonmail.com>
 *******************************************************************/

#ifndef __MOVEMENT_H__
#define __MOVEMENT_H__

#include "editor.h"

void move_left(void);
void move_right(void);
void move_up(void);
void move_down(void);

void move_home(void);
void move_end(void);
void move_npage(void);
void move_ppage(void);

#endif
