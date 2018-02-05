#include "editor.h"
#include "init.h"
#include "movement.h"

//char filename[FILENAME_MAX];

// TODO 1: fix scrolling
// TODO 2: extra window
// TODO 3: hexeditor
// TODO 4: fix buf realloc
// TODO 5: info bar
// TODO 6: valgrind memcheck

int
main(int argc, char *argv[])
{
	static char *filebuf = NULL;
	static int filebuf_size = 0;
	static int filebuf_pos = 0;

	WINDOW *win[NWINDOWS];

	int ch, i, width, height, rc;

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
			is_exit = TRUE;
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
			case CTRL_O:
			case KEY_F(4):
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
			case CTRL_K:
			case KEY_F(5):
				rc = save_file(filebuf, filebuf_size, height, width);

				if (rc == ERR) {
					fprintf(stderr, "Error in save_file() func.\n");
					is_exit = TRUE;
				}
				break;
			// Hex editor
			case CTRL_E:			// TODO: IMPLEMENT & MOVE TO EXTRAS
//				if (hex_editor() == ERR) {
//					fprintf(stderr, "Error in hex_editor.\n");
//					is_exit = TRUE;
//				}
				break;
			case KEY_F(6):
			case CTRL_G:			// TODO: MOVE TO EXTRAS
				rc = change_theme(win, height, width);

				if (rc == ERR) {
					fprintf(stderr, "Error in change_theme() func.\n");
					is_exit = TRUE;
				}
				break;
			case CTRL_Y:			// TODO: MOVE TO EXTRAS
				encryption ^= TRUE;
				break;
			// Help
			case CTRL_F:
			case KEY_F(7):
				rc = get_help(height, width);

				if (rc == ERR) {
					fprintf(stderr, "Error in get_help() func.\n");
					is_exit = TRUE;
				}
				break;
			// Exit
			case CTRL_X:
			case KEY_F(8):
				is_exit = TRUE;
				break;
			default:
				if (iscntrl(ch))
					break;

				if (encryption == TRUE)
					ch ^= 42;

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

		// Update info bar
		mvwprintw(win[2], 0, width / 2 - 6, " %3d : %3d ", curpos_y, curpos_x);
		wrefresh(win[2]);

		// Update text field
		wmove(win[1], curpos_y, curpos_x);
		touchwin(win[1]);
		wrefresh(win[1]);
	} while (!is_exit);

	free(filebuf);
	endwin();

	return 0;
}
