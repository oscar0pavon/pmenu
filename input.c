#include "input.h"

#include <linux/input-event-codes.h>
#include <wayland-client-protocol.h>

#include "items.h"
#include "pmenu.h"

//xkb_keysym_to_utf8() gives these a text of their own, so they arrive as input
//rather than as bare key codes
#define KEY_TEXT_ESCAPE 0x1B
#define KEY_TEXT_BACKSPACE 0x08
#define KEY_TEXT_DELETE 0x7F
#define KEY_TEXT_TAB 0x09

void input_text(const char *text, int length){

  if(length <= 0)
    return;

  switch(text[0]){

  case '\r':
  case '\n':
    pmenu_confirm();
    return;

  case KEY_TEXT_ESCAPE:
    pmenu_cancel();
    return;

  case KEY_TEXT_BACKSPACE:
  case KEY_TEXT_DELETE:
    items_filter_backspace();
    break;

  case KEY_TEXT_TAB:
    items_select(1);
    break;

  //ctrl+u, the line kill every readline takes. pway hands control keys over as
  //the letter minus 0x60, so there is nothing else to check against
  case 'u' - 0x60:
    items_filter_clear();
    break;

  default:
    //anything else below a space is a control key this menu has no use for.
    //appending it would put an unprintable byte in the filter and match
    //nothing from then on
    if((unsigned char)text[0] < 0x20)
      return;

    items_filter_append(text, length);
    break;
  }

  pmenu.needs_redraw = true;
}

void input_key(uint32_t key_code, uint32_t state){

  if(state != WL_KEYBOARD_KEY_STATE_PRESSED)
    return;

  switch(key_code){

  case KEY_LEFT:
    items_select(-1);
    break;

  case KEY_RIGHT:
    items_select(1);
    break;

  default:
    return;
  }

  pmenu.needs_redraw = true;
}
