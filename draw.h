#ifndef DRAW_H
#define DRAW_H

#include <stdbool.h>

//the cpu path: pway hands over the pixels of a wl_shm buffer and pfonts
//composites glyphs straight into them. no gl context is created anywhere
void draw_init(void);

//false when the compositor still owns every buffer in the pool. nothing was
//drawn in that case and the caller has to keep its redraw flag set
bool draw_menu(void);

//the height one line of the font needs, which is what the window asks to be
int draw_line_height(void);

#endif
