#include "editor.h"
#include "gui.h"
#include "init.h"
#include "readkey.h"

// TODO 1: hexeditor
// TODO 2: deal with saving && long fnames

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
