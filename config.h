#ifndef CONFIG_H
#define CONFIG_H

#include <pfonts/pfonts.h>

static const char *font_path = "/root/.fonts/fonts/ttf/FiraCode-Regular.ttf";
static const float font_size = 20;

//what sits in front of the typed text
static const char *prompt = ">";

//space around the text, and between two items, in pixels
static const int padding = 8;

//the size asked for at startup. the compositor is free to answer with another
//one - sword gives the launcher the full width of the output - and that is
//what the menu ends up drawn at
static const int default_width = 800;

static const PColor background_color = {0.13f, 0.13f, 0.13f};
static const PColor text_color = {0.85f, 0.85f, 0.85f};
static const PColor filter_color = {1.0f, 1.0f, 1.0f};
static const PColor selection_color = {0.0f, 0.33f, 0.47f};
static const PColor selection_text_color = {1.0f, 1.0f, 1.0f};

#endif
