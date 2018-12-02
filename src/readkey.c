/********************************************************************
 * PROGRAM: yate
 * FILE: readkey.c
 * PURPOSE: user input processing
 * AUTHOR: 5aboteur <5aboteur@protonmail.com>
 *******************************************************************/

#include "readkey.h"

/*
 * Function: readkey
 * -----------------
 * Description:
 *  User input handler routine.
 */

void
readkey(void)
{
	int ch = wgetch(win[EDIT_W]);

	switch (ch) {
		case KEY_LEFT:		move_left();		break;
		case KEY_RIGHT:		move_right();		break;
		case KEY_UP:		move_up();			break;
		case KEY_DOWN:		move_down();		break;
		case KEY_HOME:		move_home();		break;
		case KEY_END:		move_end();			break;
		case KEY_NPAGE:		move_npage();		break;
		case KEY_PPAGE:		move_ppage();		break;
		case KEY_DC:
		case KEY_BS:
		case KEY_BACKSPACE:	remove_char();		break;
		case KEY_ENTER:
		case KEY_NL:		next_line();		break;
		case KEY_HT:		horiz_tab();		break;
		case KEY_F(4):
		case CTRL_O:		open_file(1);		break;
		case KEY_F(5):
		case CTRL_S:		save_file();		break;
		case KEY_F(6):
		case CTRL_E:		get_extra();		break;
		case KEY_F(7):
		case CTRL_F:		get_help();			break;
		case KEY_F(8):
		case CTRL_G:		change_theme(1);	break;
//		case CTRL_W: hex_editor(win, height, width); break;
		case CTRL_X:
			finalize();
			exit(EXIT_SUCCESS);
		case CTRL_Y:		encrypt_text(1);	break;
		default:			insert_char(ch);	break;
	}
}
