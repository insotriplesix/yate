#ifndef __INIT_H__
#define __INIT_H__

#include "editor.h"

void initialize(int argc, char *argv[]);
void finalize(void);

int init_colors(void);
int init_content(char *fname);
int init_gui(void);
int init_ncurses(void);
int init_windows(void);

int load_config(void);
int save_config(void);

#endif
