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

#include "color.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

int va_buffer_size(const char *format, ...) {
	int bufsize = 0;
	va_list args;
	va_start(args, format);
	bufsize += vsnprintf(NULL, 0, format, args);
	va_end(args);

	return bufsize;
}

char *color(ansi_color_opts opts, const char *format, ...) {
	char *msg = malloc(va_buffer_size(format) * sizeof(char));
	va_list args;
	va_start(args, format);
	vsprintf(msg, format, args);
	va_end(args);

	int bufsize;
	const char *out;
	char *colorized;
	if (opts.effect == ANSI_EFFECT_NONE) {
		out = "\e[%dm%s\e[%dm\n";
		bufsize = snprintf(NULL, 0, out, opts.color, msg, ANSI_EFFECT_NONE);
		colorized = malloc(bufsize * sizeof(char));
		snprintf(colorized, bufsize, out, opts.color, msg, ANSI_EFFECT_NONE);
	} else {
		out = "\e[%d;%dm%s\e[%dm\n";
		bufsize = snprintf(NULL, 0, out, opts.color, opts.effect, msg, ANSI_EFFECT_NONE);
		colorized = malloc(bufsize * sizeof(char));
		snprintf(colorized, bufsize, out, opts.color, opts.effect, msg, ANSI_EFFECT_NONE);
	}

	free(msg);
	return colorized;
}
