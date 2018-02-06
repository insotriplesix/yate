#include "editor.h"
#include "init.h"
#include "movement.h"

//char filename[FILENAME_MAX];

// TODO 1: fix scrolling
// TODO 2: hexeditor
// TODO 3: fix buf realloc
// TODO 4: info bar
// TODO 5: valgrind memcheck

int
main(int argc, char *argv[])
{
	static char *filebuf = NULL;
	static int filebuf_size = 0;
	static int filebuf_pos = 0;

	WINDOW *win[NWINDOWS];

	int ch, i, width, height, rc = OK;

	if (init_ncurses() == ERR || init_colors() == ERR) {
		fprintf(stderr, "Cannot initialize ncurses library.\n");
		endwin();
		exit(EXIT_FAILURE);
	}

	getmaxyx(stdscr, height, width);

	if (init_windows(win, height, width) == ERR) {
		fprintf(stderr, "Cannot initialize windows.\n");
		endwin();
		exit(EXIT_FAILURE);
	}

	filebuf = calloc(BUFSIZ, sizeof(char));

	int defpos_y = 1, defpos_x = 1;
	int curpos_y = defpos_y, curpos_x = defpos_x;

	int is_exit = FALSE;
	int encryption = FALSE;

	if (argc == 2) {
		strcpy(filename, argv[1]);
		rc = open_file(filebuf, &filebuf_size, height, width, TRUE);

		if (rc == ERR) {
			fprintf(stderr, "Error in open_file() func.\n");
			free(filebuf);
			endwin();
			return rc;
//			is_exit = TRUE;
		}

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
				move_right(win[1], &curpos_y, &curpos_x, width);
			}
		}

		wrefresh(win[1]);
	}

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

				filebuf_pos = curpos_y * width + curpos_x;
				if (filebuf_size < BUFSIZ || filebuf_pos < filebuf_size)
					filebuf[filebuf_size--] = ' ';

				break;
			// Next line
			case KEY_ENTER:
			case KEY_NL:
				filebuf_pos = curpos_y * width + curpos_x;
				if (filebuf_size < BUFSIZ || filebuf_pos < filebuf_size)
					filebuf[filebuf_size++] = '\n';

				curpos_x = defpos_x;
				++curpos_y;
				break;
			// Horizontal tab
			case KEY_HT:
				filebuf_pos = curpos_y * width + curpos_x;
				if (filebuf_size < BUFSIZ || filebuf_pos < filebuf_size)
					filebuf[filebuf_size++] = '\t';

				for (i = 0; i < 4; ++i)
					move_right(win[1], &curpos_y, &curpos_x, width - 1);

				break;
			// Open file
			case KEY_F(4):
			case CTRL_O:
				rc = open_file(filebuf, &filebuf_size, height, width, FALSE);

				if (rc == ERR) {
					fprintf(stderr, "Error in open_file() func.\n");
					is_exit = TRUE;
				}

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
						move_right(win[1], &curpos_y, &curpos_x, width);
					}
				}
				break;
			// Save to file
			case KEY_F(5):
			case CTRL_K:
				rc = save_file(filebuf, filebuf_size, height, width);

				if (rc == ERR) {
					fprintf(stderr, "Error in save_file() func.\n");
					is_exit = TRUE;
				}
				break;
			// Extra
			case KEY_F(6):
			case CTRL_E:
				rc = get_extra(height, width);

				if (rc == ERR) {
					fprintf(stderr, "Error in get_extra() func.\n");
					is_exit = TRUE;
				}
				break;
			// Help
			case KEY_F(7):
			case CTRL_F:
				rc = get_help(height, width);

				if (rc == ERR) {
					fprintf(stderr, "Error in get_help() func.\n");
					is_exit = TRUE;
				}
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
				rc = change_theme(win, height, width);

				if (rc == ERR) {
					fprintf(stderr, "Error in change_theme() func.\n");
					is_exit = TRUE;
				}
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

				filebuf_pos = curpos_y * width + curpos_x;
				if (filebuf_size < BUFSIZ || filebuf_pos < filebuf_size)
					filebuf[filebuf_size++] = (char) ch;

				if (curpos_x + 1 < width - 1) {
					waddch(win[1], ch);
					move_right(win[1], &curpos_y, &curpos_x, width);
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
		mvwprintw(win[2], 0, width / 2 - 6, " %3d : %3d ", curpos_y, curpos_x);
		touchwin(win[2]);
		wrefresh(win[2]);

		// Update text field
		wmove(win[1], curpos_y, curpos_x);
		touchwin(win[1]);
		wrefresh(win[1]);

	} while (!is_exit);

	free(filebuf);
	endwin();

	return rc;
}
