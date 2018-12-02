/********************************************************************
 * PROGRAM: yate
 * FILE: readkey.h
 * PURPOSE: user input processing
 * AUTHOR: 5aboteur <5aboteur@protonmail.com>
 *******************************************************************/

#ifndef __READKEY_H__
#define __READKEY_H__

#include "gui.h"
#include "init.h"
#include "editor.h"
#include "movement.h"

#define KEY_BS '\b'
#define KEY_CR '\r'
#define KEY_HT '\t'
#define KEY_NL '\n'

#define CTRL_E 5
#define CTRL_F 6
#define CTRL_G 7
#define CTRL_O 15
#define CTRL_S 19
#define CTRL_W 23
#define CTRL_X 24
#define CTRL_Y 25

void readkey(void);

#endif
