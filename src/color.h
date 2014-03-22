#ifndef COLOR_H
#define COLOR_H

typedef enum {
    ANSI_COLOR_BLACK = 30,
    ANSI_COLOR_RED = 31,
    ANSI_COLOR_GREEN = 32,
    ANSI_COLOR_YELLOW = 33,
    ANSI_COLOR_BLUE = 34,
    ANSI_COLOR_MAGENTA = 35,
    ANSI_COLOR_CYAN = 36,
    ANSI_COLOR_WHITE = 37,
    ANSI_COLOR_PLAIN = 39,
    ANSI_COLOR_GRAY = 90
} ansi_color;

typedef enum {
    ANSI_EFFECT_NONE = 0,
    ANSI_EFFECT_BOLD = 1,
    ANSI_EFFECT_UNDERLINE = 4,
} ansi_effect;

typedef struct {
    ansi_color color;
    ansi_effect effect;
} ansi_color_opts;

extern char *color(ansi_color_opts opts, const char *format, ...);

#endif
