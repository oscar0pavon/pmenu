#ifndef PMENU_H
#define PMENU_H

#include <stdbool.h>

//the whole program's state. a menu is one filter string, the items that match
//it and which of those is selected - everything else is drawing
typedef struct PMenu{
  bool is_running;
  //the frame is not redrawn on a timer: only a keystroke changes what is on
  //screen, and draw_menu() declines when the compositor still owns every
  //buffer, so the flag stays set and the next loop iteration retries
  bool needs_redraw;
  bool needs_resize;
  int width;
  int height;
  //a menu that has lost the keyboard is a menu nobody can type into. it closes
  //rather than sitting on top of the window that took the focus - but only
  //once it has actually been focused, or the leave that arrives before the
  //first enter would close it on the way up
  bool was_focused;
}PMenu;

extern PMenu pmenu;

//print the selected item and leave. nothing is written on a cancel, so a
//caller piping into a shell runs nothing
void pmenu_confirm(void);
void pmenu_cancel(void);

#endif
