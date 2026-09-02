#include "items.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILTER_MAX 256

static char **entries;
static int entry_count;

//indices into entries, in the order they are shown
static int *matches;
static int match_count;
static int selected;

static char filter[FILTER_MAX];
static int filter_length;

//stdin is the whole interface: one item per line, exactly as dmenu takes it.
//the caller is what decides whether those lines are programs in $PATH, open
//windows or anything else, which is why this program has no idea what they are
void items_read(void){

  size_t capacity = 0;
  char *line = NULL;
  ssize_t length;

  while((length = getline(&line, &capacity, stdin)) != -1){

    if(length > 0 && line[length - 1] == '\n')
      line[length - 1] = '\0';

    if(line[0] == '\0')
      continue;

    entries = realloc(entries, sizeof(*entries) * (entry_count + 1));
    entries[entry_count++] = strdup(line);
  }

  free(line);

  matches = malloc(sizeof(*matches) * (entry_count > 0 ? entry_count : 1));

  items_filter();
}

void items_free(void){

  for(int i = 0; i < entry_count; i++)
    free(entries[i]);

  free(entries);
  free(matches);
}

static bool starts_with(const char *text, const char *pattern){

  for(int i = 0; pattern[i]; i++){
    if(tolower((unsigned char)text[i]) != tolower((unsigned char)pattern[i]))
      return false;
  }

  return true;
}

static bool contains(const char *text, const char *pattern){

  for(int i = 0; text[i]; i++){
    if(starts_with(text + i, pattern))
      return true;
  }

  return false;
}

//two passes rather than a sort: the prefixes go in first and keep the order
//they were read in, then everything else that merely contains the filter. an
//empty filter matches everything on the first pass, since every string starts
//with nothing
void items_filter(void){

  match_count = 0;

  for(int i = 0; i < entry_count; i++){
    if(starts_with(entries[i], filter))
      matches[match_count++] = i;
  }

  for(int i = 0; i < entry_count; i++){
    if(!starts_with(entries[i], filter) && contains(entries[i], filter))
      matches[match_count++] = i;
  }

  selected = 0;
}

void items_filter_append(const char *text, int length){

  if(filter_length + length >= FILTER_MAX)
    return;

  memcpy(filter + filter_length, text, length);
  filter_length += length;
  filter[filter_length] = '\0';

  items_filter();
}

//a utf8 continuation byte is not a character of its own, so a backspace walks
//back over the whole sequence rather than leaving half of one behind
void items_filter_backspace(void){

  while(filter_length > 0){

    filter_length--;

    if((filter[filter_length] & 0xC0) != 0x80)
      break;
  }

  filter[filter_length] = '\0';

  items_filter();
}

void items_filter_clear(void){

  filter_length = 0;
  filter[0] = '\0';

  items_filter();
}

//the selection stops at both ends instead of wrapping: with the matches drawn
//in a row, wrapping from the last back to the first jumps the highlight across
//the whole bar and reads as the list having changed
void items_select(int direction){

  int next = selected + direction;

  if(next >= 0 && next < match_count)
    selected = next;
}

const char *items_filter_text(void){
  return filter;
}

const char *items_at(int match_index){

  if(match_index < 0 || match_index >= match_count)
    return NULL;

  return entries[matches[match_index]];
}

int items_match_count(void){
  return match_count;
}

int items_selected(void){
  return selected;
}

const char *items_result(void){

  const char *item = items_at(selected);

  return item ? item : filter;
}
