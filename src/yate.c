#include "editor.h"
#include "gui.h"
#include "init.h"
#include "readkey.h"

// TODO 1: fix scrolling
// TODO 2: hexeditor
// TODO 3: info bar
// TODO 4: deal with resizing
// TODO 5: valgrind -v --leak-check=full ./yate FILE
// TODO 6: add .config file (win size, theme etc)

int
main(int argc, char *argv[])
{
	int rc = OK;

	initialize();

	if (argc == 2) {
		strcpy(filename, argv[1]);
		rc = open_file(TRUE);

		if (rc == ERR)
			return ERR;

		print_text();

		wmove(win[EDIT_W], content.y_pos, content.x_pos);
		wrefresh(win[EDIT_W]);
	}

	while (0x1) {
		rc = readkey();
		if (rc == ERR) break;
		update_gui();
	}

	finalize();

	return rc;
}
