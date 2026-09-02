#include <stdio.h>
#include <stdlib.h>

#include <pfonts/pfonts.h>
#include <pway/pway.h>

#include "config.h"
#include "draw.h"
#include "input.h"
#include "items.h"
#include "pmenu.h"

PMenu pmenu;

//the app id is the whole reason sword puts this window across the top of the
//output instead of handing it a tile. it is set before the first commit, which
//is where a compositor expects to be told what a client is
#define PMENU_APP_ID "pmenu"

void pmenu_confirm(void){

  printf("%s\n", items_result());
  fflush(stdout);

  pmenu.is_running = false;
}

//nothing is printed, so a caller piping this into a shell runs nothing at all
void pmenu_cancel(void){
  pmenu.is_running = false;
}

static void window_resized(int width, int height){

  if(width == 0 || height == 0)
    return;

  if(width == pmenu.width && height == pmenu.height)
    return;

  pmenu.width = width;
  pmenu.height = height;

  //the pool is reallocated at the top of the next draw, not here: this runs
  //inside wayland event dispatch, where a buffer may still be checked out
  pmenu.needs_resize = true;
  pmenu.needs_redraw = true;
}

static void window_closed(void){
  pmenu.is_running = false;
}

//a menu nobody can type into has nothing left to do. the guard is what keeps
//the leave that can arrive before the first enter from closing it on the way
//up
static void window_focus(bool is_focused){

  if(is_focused){
    pmenu.was_focused = true;
    return;
  }

  if(pmenu.was_focused)
    pmenu_cancel();
}

static void create_window(void){

  pmenu.width = default_width;
  pmenu.height = draw_line_height();

  pway = pway_init();

  //NULL is pway saying it never reached a compositor. every callback below
  //writes through this pointer, so there is nothing to do but leave
  if(!pway){
    fprintf(stderr, "pmenu: no wayland compositor\n");
    exit(EXIT_FAILURE);
  }

  pway->input = input_text;
  pway->key = input_key;
  pway->resize = window_resized;
  pway->exit = window_closed;
  pway->focus = window_focus;

  pway_set_app_id(PMENU_APP_ID);

  if(!pway_create_window("pmenu", pmenu.width, pmenu.height)){
    fprintf(stderr, "pmenu: can't create the window\n");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char **argv){

  items_read();

  //the window is sized off the font, so the metrics have to exist before it
  if(!pfonts_load_font(font_path, font_size)){
    fprintf(stderr, "pmenu: can't load font %s\n", font_path);
    return EXIT_FAILURE;
  }

  create_window();
  draw_init();

  pmenu.is_running = true;
  pmenu.needs_redraw = true;

  while(pmenu.is_running){

    pway_handle_events();

    //the compositor can go away inside the event handling, and there is no
    //surface left to draw into after that
    if(!pmenu.is_running || pway_connection_lost())
      break;

    if(pmenu.needs_redraw)
      pmenu.needs_redraw = !draw_menu();
  }

  pway_finish();
  items_free();

  return EXIT_SUCCESS;
}
