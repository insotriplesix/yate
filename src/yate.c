#include "editor.h"
#include "gui.h"
#include "init.h"
#include "readkey.h"

// TODO 1: fix scrolling
// TODO 2: hexeditor
// TODO 3: deal with saving && long fnames
// TODO 4: valgrind -v --leak-check=full --show-leak-kinds=all ./yate [file]

int
main(int argc, char *argv[])
{
	int rc = OK;

	initialize(argc, argv);

	while (0x1) {
		update_gui();
		rc = readkey();
		if (rc == ERR) break;
	}

	finalize();

	return rc;
}
