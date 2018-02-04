#include "editor.h"
#include "init.h"
#include "movement.h"

int
main(int argc, char *argv[])
{
	static char *filebuf = NULL;
	static int filebuf_len = 0;

	WINDOW *win[NWINDOWS];

	int ch, i, width, height;

	if (init_ncurses() == ERR || init_colors() == ERR)
	{
		fprintf(stderr, "Cannot initialize ncurses.\n");
		endwin();
		exit(EXIT_FAILURE);
	}

	getmaxyx(stdscr, height, width);

	if (init_windows(win, height, width) == ERR)
	{
		fprintf(stderr, "Cannot initialize windows.\n");
		endwin();
		exit(EXIT_FAILURE);
	}

	filebuf = malloc(sizeof(char) * BUFSIZ);

	int defpos_y = 1, defpos_x = 1;
	int curpos_y = defpos_y, curpos_x = defpos_x;

	int is_exit = FALSE;
	int encryption = FALSE;

	do {
		ch = wgetch(win[1]);

		switch (ch) {
			// Movements
			case KEY_LEFT:
				move_left(win[1], &curpos_y, &curpos_x, 0);
				break;
			case KEY_RIGHT:
				move_right(win[1], &curpos_y, &curpos_x, width - 1);
				break;
			case KEY_UP:
				move_up(win[1], &curpos_y, &curpos_x, 0);
				break;
			case KEY_DOWN:
				move_down(win[1], &curpos_y, &curpos_x, height - 9);
				break;
			case KEY_HOME:
				curpos_x = defpos_x;
				break;
			case KEY_END:
				curpos_x = width - 2;
				break;
			// Remove character
			case KEY_DC:
			case KEY_BS:
			case KEY_BACKSPACE:
				move_left(win[1], &curpos_y, &curpos_x, 0);
				waddch(win[1], ' ');

				// Also erase it from the buffer
				if (filebuf_len && (curpos_x > 1)) {
					filebuf[filebuf_len--] = ' ';
				}
				break;
			// Next line
			case KEY_ENTER:
			case KEY_NL:			// NOT CORRECT!!!
				curpos_x = defpos_x;
				++curpos_y;
				filebuf[filebuf_len++] = '\n';	// buf position?
				break;
			// Horizontal tab
			case KEY_HT:
				for (i = 0; i < 4; ++i) {
					move_right(win[1], &curpos_y, &curpos_x, width - 1);
				}
				break;
			// Open file
			case CTRL_O:			// NOT CORRECT!!!
			case KEY_F(4):
				open_file(filebuf, &filebuf_len, height, width);
				mvwinsstr(win[1], 1, 1, filebuf);
				break;
			// Save to file
			case CTRL_K:
			case KEY_F(5):			// IS CORRECT???
				save_file(filebuf, filebuf_len, height, width);
				break;
			// Hex editor
			case CTRL_E:			// TODO: IMPLEMENT & MOVE TO EXTRAS
//				if (hex_editor() == ERR) {
//					fprintf(stderr, "Error in hex_editor().\n");
//					is_exit = TRUE;
//				}
				break;
			case KEY_F(6):
			case CTRL_G:			// TODO: MOVE TO EXTRAS
				if (change_theme(win, height, width) == ERR) {
					fprintf(stderr, "Error in change_theme().\n");
					is_exit = TRUE;
				}
				break;
			case CTRL_Y:			// TODO: MOVE TO EXTRAS
				encryption ^= TRUE;
				break;
			// Help
			case CTRL_F:
			case KEY_F(7):
				if (get_help(height, width) == ERR) {
					fprintf(stderr, "Error in get_help().\n");
					is_exit = TRUE;
				}
				break;
			// Exit
			case CTRL_X:
			case KEY_F(8):
				is_exit = TRUE;
				break;
			default:
				if (iscntrl(ch)) break;
				if (encryption == TRUE) ch ^= 1;

				filebuf[filebuf_len++] = (char) ch;

				// Deal with eol
				if (curpos_x + 1 < width - 1) {
					waddch(win[1], ch);
					move_right(win[1], &curpos_y, &curpos_x, width);
				} else {
					curpos_x = defpos_x;
					waddch(win[1], ch);
					++curpos_y;
				}
		}

		// Dump the changed content on the screen
		mvwprintw(win[2], 0, width / 2 - 6, " %3d : %3d ", curpos_y, curpos_x);
		// filename changed etc.
		wrefresh(win[2]);

		// Update text field
		wmove(win[1], curpos_y, curpos_x);
		touchwin(win[1]);
		wrefresh(win[1]);
	} while (is_exit == FALSE);

	free(filebuf);
	endwin();

	return 0;
}
