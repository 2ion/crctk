/*
  Copyright (c) 2014 modocache
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

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
