/********************************************************************
 * PROGRAM: yate
 * FILE: yate.c
 * PURPOSE: main routine
 * AUTHOR: 5aboteur <5aboteur@protonmail.com>
 *******************************************************************/

#include "editor.h"
#include "gui.h"
#include "init.h"
#include "readkey.h"

// TODO: built-in hexeditor

int
main(int argc, char *argv[])
{
	initialize(argc, argv);

	while (0x1) {
		update_gui();
		readkey();
	}

	finalize();

	return 0;
}
