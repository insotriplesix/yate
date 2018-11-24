#include "readkey.h"

int
readkey(void)
{
	int ch, rc = OK;

	ch = wgetch(win[EDIT_W]);

	switch (ch) {
		case KEY_LEFT:
			move_left();
			break;
		case KEY_RIGHT:
			move_right();
			break;
		case KEY_UP:
			move_up();
			if (content.y_pos == DEFPOS_Y && content.y_off > 0)
				print_text();
			break;
		case KEY_DOWN:
			move_down();
			if (content.y_pos == LINES - 10)
				print_text();
			break;
		case KEY_HOME:
			content.x_pos = DEFPOS_X;
			break;
		case KEY_END:
			content.x_pos = COLS - 2;
			break;
		case KEY_DC:
		case KEY_BS:
		case KEY_BACKSPACE:
			remove_char();
			break;
		case KEY_ENTER:
		case KEY_NL:
			next_line();
			break;
		case KEY_HT:
			horizontal_tab();
			break;
		case KEY_F(4):
		case CTRL_O:
			rc = open_file_ed();
			if (rc == ERR) break;
			print_text();
			break;
		case KEY_F(5):
		case CTRL_S:
			rc = save_file();
			break;
		case KEY_F(6):
		case CTRL_E:
			rc = get_extra_opt();
			break;
		case KEY_F(7):
		case CTRL_F:
			rc = get_help();
			break;
		case KEY_F(8):
		case CTRL_X:
			finalize();
			exit(EXIT_SUCCESS);
			break;
			/*case CTRL_W:
			rc = hex_editor(win, height, width);
			break; */
		case CTRL_G:
			rc = change_theme_ed();
			break;
		case CTRL_Y:
			encryption ^= TRUE;
			wclear(win[INFO_W]);
			draw_window(INFO_W);
			break;
		default:
			print_char(ch);
	}

	return rc;
}
