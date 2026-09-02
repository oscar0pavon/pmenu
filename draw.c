#include "draw.h"

#include <string.h>

#include <pfonts/pfonts_cpu.h>
#include <pway/pway.h>

#include "config.h"
#include "items.h"
#include "pmenu.h"

void draw_init(void){

  pway_init_shm();

  //the atlas the cpu backend rasterizes into. pfonts_init() is the gl
  //counterpart and would want a context; this one wants nothing
  pfonts_cpu_init();
}

int draw_line_height(void){
  return (int)pfonts_get_cell_height() + padding * 2;
}

//one utf8 sequence, returned as the codepoint with the cursor left on the byte
//after it. the items are program names and are ascii, but the filter is
//whatever was typed into it
static uint32_t next_codepoint(const char *text, int *index){

  unsigned char byte = text[*index];
  int extra = 0;
  uint32_t codepoint = byte;

  if(byte >= 0xF0){
    extra = 3;
    codepoint = byte & 0x07;
  }else if(byte >= 0xE0){
    extra = 2;
    codepoint = byte & 0x0F;
  }else if(byte >= 0xC0){
    extra = 1;
    codepoint = byte & 0x1F;
  }

  (*index)++;

  for(int i = 0; i < extra && (text[*index] & 0xC0) == 0x80; i++){
    codepoint = (codepoint << 6) | (text[*index] & 0x3F);
    (*index)++;
  }

  return codepoint;
}

static float text_width(const char *text){

  float width = 0;
  int index = 0;

  while(text[index])
    width += pfonts_get_glyph_advance(next_codepoint(text, &index));

  return width;
}

//the cpu backend rasterizes one glyph at a time - there is no string call on
//that side of pfonts - so the pen is walked here. returns where it ended up,
//which is what puts the matches after the text that was typed
static float draw_text(const char *text, PColor color, float x, float y){

  int index = 0;

  while(text[index]){

    uint32_t codepoint = next_codepoint(text, &index);

    pfonts_cpu_draw_glyph(codepoint, color, x, y);

    x += pfonts_get_glyph_advance(codepoint);
  }

  return x;
}

//the pen sits on the baseline, so the line is centered by putting the ascent
//below the top of the leftover space rather than by centering the cell
static float baseline(void){

  float line = pfonts_get_ascent() - pfonts_get_descent();

  return (pmenu.height - line) / 2 + pfonts_get_ascent();
}

static float draw_filter(float x, float y){

  x = draw_text(prompt, text_color, x, y);
  x += padding;

  return draw_text(items_filter_text(), filter_color, x, y);
}

//the matches run left to right from the end of what was typed, dmenu's own
//layout: the strip is one line tall, so a vertical list has nowhere to go.
//whatever does not fit is simply not drawn - the selection is moved with the
//arrows rather than by scrolling to it
static void draw_matches(float x, float y){

  int count = items_match_count();

  for(int i = 0; i < count; i++){

    const char *item = items_at(i);
    float width = text_width(item);

    if(x + width > pmenu.width)
      return;

    PColor color = text_color;

    if(i == items_selected()){

      pfonts_cpu_draw_rect(selection_color, x - padding / 2, 0,
                           width + padding, pmenu.height);

      color = selection_text_color;
    }

    draw_text(item, color, x, y);

    x += width + padding * 2;
  }
}

bool draw_menu(void){

  if(pmenu.needs_resize){

    //the pool is destroyed and built again here rather than where the
    //configure arrived: nothing may still be holding a pointer from
    //pway_shm_get_buffer() when it goes
    pway_shm_resize(pmenu.width, pmenu.height);
    pmenu.needs_resize = false;
  }

  int stride;
  uint32_t *pixels = pway_shm_get_buffer(&stride);

  if(!pixels)
    return false;

  //stride is in bytes and the target wants words
  pfonts_cpu_set_target(pixels, pmenu.width, pmenu.height, stride / 4);

  pfonts_cpu_draw_rect(background_color, 0, 0, pmenu.width, pmenu.height);

  float y = baseline();
  float x = draw_filter(padding, y);

  draw_matches(x + padding * 3, y);

  pway_shm_commit(0, 0, pmenu.width, pmenu.height);

  return true;
}
