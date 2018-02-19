#include "gui.h"
#include "init.h"

char filename[FILENAME_MAX];

void
finalize(void)
{
	disable_raw_mode();
	free(content.data);

	for (int i = 0; i < NWINDOWS; ++i)
		delwin(win[i]);

	endwin();
	save_config();
}

void
initialize(int argc, char *argv[])
{
	if (init_ncurses() | init_colors() | init_windows() | init_gui()) {
		endwin();
		fprintf(stderr, "Initializing error.\n");
		exit(EXIT_FAILURE);
	}

	if (LINES < 11 || COLS < 98) {
		endwin();
		fprintf(stderr, "Terminal window is too small.\n"
			"Min: 11x98, your: %dx%d\n", LINES, COLS);
		exit(EXIT_FAILURE);
	}

	if (argc > 2) {
		endwin();
		fprintf(stderr, "Usage: ./yate [file]\n");
		exit(EXIT_FAILURE);
	} else if (argc == 2) {
		init_content(argv[1]);
		open_file();
		print_text();
	} else {
		init_content("yate_untitled.txt");
	}

	wclear(win[INFO_W]);
	draw_window(INFO_W);

	load_config();
	enable_raw_mode();

	wmove(win[EDIT_W], DEFPOS_Y, DEFPOS_X);
}

int
init_content(char *fname)
{
	content.x_pos = DEFPOS_X;
	content.y_pos = DEFPOS_Y;
	content.y_off = 0;
	content.buf_pos = 0;
	content.size = 0;

	strcpy(content.file.name, fname);
	content.data = calloc(BUFSIZ, sizeof(char));

	encryption = FALSE;

	return OK;
}

int
init_colors(void)
{
	if (start_color() == ERR || !has_colors()) {
		fprintf(stderr, "This terminal doesn't support colouring.\n");
		return ERR;
	}

	// Pair id, Foreground, Background
	init_pair(1, COLOR_BLACK, COLOR_YELLOW);	// menu components
	init_pair(2, COLOR_WHITE, COLOR_BLUE);		// edit field
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);	// pop-up windows
	init_pair(4, COLOR_WHITE, COLOR_BLACK);		// borders

	return OK;
}

int
init_gui(void)
{
	for (int i = 0; i < NWINDOWS; ++i)
		draw_window(i);

	return OK;
}

int
init_ncurses(void)
{
	return (initscr() != NULL) & clear() & cbreak() & noecho();
}

int
init_windows(void)
{
	win[MENU_W] = newwin(3, COLS, 0, 0);
	if (win[MENU_W] == NULL) {
		perror("newwin");
		return ERR;
	}

	win[EDIT_W] = newwin(LINES - 8, COLS, 3, 0);
	if (win[EDIT_W] == NULL) {
		perror("newwin");
		return ERR;
	}

	win[INFO_W] = newwin(5, COLS, LINES - 5, 0);
	if (win[INFO_W] == NULL) {
		perror("newwin");
		return ERR;
	}

	// Enable scrolling, func keys, arrows etc.
	keypad(win[EDIT_W], TRUE);

	return OK;
}

int
load_config(void)
{
	FILE *fp = fopen(CONFIG_FILE, "r");
	if (fp == NULL)	return ERR;

	char line[LINE_MAX];
	char theme = 0;

	while ((fgets(line, LINE_MAX, fp)) != NULL) {
		if (strncmp(line, "theme", 5) == 0) {
			char **str = split_s(line, ' ');

			if (str != NULL) {
				theme = str[1][0];
				change_theme(theme);
			}

			for (int i = 0; i < 2; ++i)
				free(str[i]);
			free(str);
		}
	}

	fclose(fp);

	return OK;
}

int
save_config(void)
{
	FILE *fp = fopen(CONFIG_FILE, "w");
	if (fp == NULL)	return ERR;

	fprintf(fp, "theme %c\n", current_theme);
	fclose(fp);

	return OK;
}
