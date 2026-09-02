#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

//the text of a keypress, already utf8 encoded by pway. control keys arrive
//here too - a return is "\r" - which is why this handles more than typing
void input_text(const char *text, int length);

//the raw evdev code, for the keys that encode to no text at all. presses and
//releases both, so the state has to be checked
void input_key(uint32_t key_code, uint32_t state);

#endif
