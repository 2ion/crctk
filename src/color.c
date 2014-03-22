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
