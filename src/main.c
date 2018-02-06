#include "editor.h"
#include "init.h"
#include "movement.h"

// TODO 1: fix scrolling
// TODO 2: hexeditor
// TODO 3: info bar
// TODO 4: lots of refactoring stuff (e.g.: open via arg)
// TODO 5: valgrind memcheck

int
main(int argc, char *argv[])
{
	char *filebuf = NULL;
	int filebuf_size = 0;
	int filebuf_pos = 0;

	int ch, i, rc = OK;

	if (init_ncurses() | init_colors() | init_windows()) {
		endwin();
		fprintf(stderr, "Initialize stage error.\n");
		exit(EXIT_FAILURE);
	}

	if (LINES < 11 || COLS < 98) {
		endwin();
		fprintf(stderr, "Terminal window is too small.\n"
			"Min: 11x98, your: %dx%d\n", LINES, COLS);
		exit(EXIT_FAILURE);
	}

	filebuf = calloc(BUFSIZ, sizeof(char));

	int defpos_y = 1, defpos_x = 1;
	int curpos_y = defpos_y, curpos_x = defpos_x;

	int is_exit = FALSE;
	int encryption = FALSE;

	if (argc == 2) {
		strcpy(filename, argv[1]);
		rc = open_file(&filebuf, &filebuf_size, TRUE);

		if (rc == ERR) is_exit = TRUE;
		else {
			// Repaint window to clear previous stuff
			wclear(win[1]);
			wattron(win[1], COLOR_PAIR(4));
		    box(win[1], ACS_VLINE, ACS_HLINE);
		    wattroff(win[1], COLOR_PAIR(4));

			wmove(win[1], curpos_y, curpos_x);

			// Dump text to the screen
			for (int i = 0; i < filebuf_size; ++i) {
				if (filebuf[i] == '\n') {
					curpos_x = defpos_x;
					++curpos_y;
					wmove(win[1], curpos_y, curpos_x);
				} else {
					waddch(win[1], filebuf[i]);
					move_right(&curpos_y, &curpos_x, COLS);
				}
			}

			wrefresh(win[1]);
		}
	}

	while (!is_exit) {
		ch = wgetch(win[1]);

		switch (ch) {
			// Movements
			case KEY_LEFT:
				move_left(&curpos_y, &curpos_x, 0);
				break;
			case KEY_RIGHT:
				move_right(&curpos_y, &curpos_x, COLS - 1);
				break;
			case KEY_UP:
				move_up(&curpos_y, &curpos_x, 0);
				break;
			case KEY_DOWN:
				move_down(&curpos_y, &curpos_x, LINES - 9);
				break;
			case KEY_HOME:
				curpos_x = defpos_x;
				break;
			case KEY_END:
				curpos_x = COLS - 2;
				break;
			// Remove character
			case KEY_DC:
			case KEY_BS:
			case KEY_BACKSPACE:
				move_left(&curpos_y, &curpos_x, 0);
				waddch(win[1], ' ');

				filebuf_pos = curpos_y * COLS + curpos_x;
				if (filebuf_size < BUFSIZ || filebuf_pos < filebuf_size)
					filebuf[filebuf_size--] = ' ';

				break;
			// Next line
			case KEY_ENTER:
			case KEY_NL:
				filebuf_pos = curpos_y * COLS + curpos_x;
				if (filebuf_size < BUFSIZ || filebuf_pos < filebuf_size)
					filebuf[filebuf_size++] = '\n';

				curpos_x = defpos_x;
				++curpos_y;
				break;
			// Horizontal tab
			case KEY_HT:
				filebuf_pos = curpos_y * COLS + curpos_x;
				if (filebuf_size < BUFSIZ || filebuf_pos < filebuf_size)
					filebuf[filebuf_size++] = '\t';

				for (i = 0; i < 4; ++i)
					move_right(&curpos_y, &curpos_x, COLS - 1);

				break;
			// Open file
			case KEY_F(4):
			case CTRL_O:
				rc = open_file(&filebuf, &filebuf_size, FALSE);

				if (rc == ERR) is_exit = TRUE;

				curpos_x = defpos_x;
				curpos_y = defpos_y;

				// Repaint window
				wclear(win[1]);
				wattron(win[1], COLOR_PAIR(4));
			    box(win[1], ACS_VLINE, ACS_HLINE);
			    wattroff(win[1], COLOR_PAIR(4));

				wmove(win[1], curpos_y, curpos_x);

				// Dump text
				for (int i = 0; i < filebuf_size; ++i) {
					if (filebuf[i] == '\n') {
						curpos_x = defpos_x;
						++curpos_y;
						wmove(win[1], curpos_y, curpos_x);
					} else {
						waddch(win[1], filebuf[i]);
						move_right(&curpos_y, &curpos_x, COLS);
					}
				}
				break;
			// Save to file
			case KEY_F(5):
			case CTRL_K:
				rc = save_file(filebuf, filebuf_size);
				if (rc == ERR) is_exit = TRUE;
				break;
			// Extra
			case KEY_F(6):
			case CTRL_E:
				rc = get_extra();
				if (rc == ERR) is_exit = TRUE;
				break;
			// Help
			case KEY_F(7):
			case CTRL_F:
				rc = get_help();
				if (rc == ERR) is_exit = TRUE;
				break;
			// Exit
			case KEY_F(8):
			case CTRL_X:
				is_exit = TRUE;
				break;
/*			case CTRL_W:
				rc = hex_editor(win, height, width);

				if (rc == ERR) {
					fprintf(stderr, "Error in hex_editor() func.\n");
					is_exit = TRUE;
				}
				break;
*/			// Change theme
			case CTRL_G:
				rc = change_theme();
				if (rc == ERR) is_exit = TRUE;
				break;
			// Enable live encryption
			case CTRL_Y:
				encryption ^= TRUE;
				break;
			// Print character
			default:
				if (iscntrl(ch))
					break;

				if (encryption == TRUE)
					ch ^= 1;

				filebuf_pos = curpos_y * COLS + curpos_x;
				if (filebuf_size < BUFSIZ || filebuf_pos < filebuf_size)
					filebuf[filebuf_size++] = (char) ch;

				if (curpos_x + 1 < COLS - 1) {
					waddch(win[1], ch);
					move_right(&curpos_y, &curpos_x, COLS);
				} else {
					// Deal with eol
					waddch(win[1], ch);
					curpos_x = defpos_x;
					++curpos_y;
				}
		}

		// Update options bar
		touchwin(win[0]);
		wrefresh(win[0]);

		// Update info bar
		mvwprintw(win[2], 0, COLS / 2 - 6, " %3d : %3d ", curpos_y, curpos_x);
		touchwin(win[2]);
		wrefresh(win[2]);

		// Update text field
		wmove(win[1], curpos_y, curpos_x);
		touchwin(win[1]);
		wrefresh(win[1]);
	}

	free(filebuf);
	endwin();

	return rc;
}
