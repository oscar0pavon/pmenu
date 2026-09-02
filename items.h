#ifndef ITEMS_H
#define ITEMS_H

//the list the menu is choosing from: every line read from stdin, whichever of
//them match what has been typed, and which match is selected
void items_read(void);
void items_free(void);

//rebuild the match list from the filter. a prefix match sorts before a
//substring one, so typing "fire" puts firefox first and leaves the things that
//merely contain it further along
void items_filter(void);

void items_filter_append(const char *text, int length);
void items_filter_backspace(void);
void items_filter_clear(void);

void items_select(int direction);

const char *items_filter_text(void);
const char *items_at(int match_index);
int items_match_count(void);
int items_selected(void);

//what the menu prints on enter: the selected item, or the filter itself when
//nothing matched - the same thing dmenu does, so a command that is not in the
//list can still be typed and run
const char *items_result(void);

#endif
