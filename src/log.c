/*
 * crctk - CRC32 Hexstring Toolkit
 * Copyright (C) 2014 Jens Oliver John <dev@2ion.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * */

#include "log.h"

static char LOG_FUNC_BUFFER[256];

static int LOG(FILE *file, const char *color, const char *module, const char *format, va_list ap) {
  va_list arg;
  int done;
  int msize;
  char *fmt = LOG_FUNC_BUFFER;

  msize = snprintf(NULL, 0, "[%s%s%s] %s\n\n", IFCOLORS(color),
      module, IFCOLORS(ANSI_COLOR_RESET), format);
  if(msize > sizeof(LOG_FUNC_BUFFER)
      && (fmt = malloc(msize)) == NULL)
      LERROR(EXIT_FAILURE, errno, "malloc() failed");
  snprintf(fmt, msize, "[%s%s%s] %s\n\n", IFCOLORS(color),
      module, IFCOLORS(ANSI_COLOR_RESET), format);

  done = vfprintf(file, fmt, ap);

  if(fmt != LOG_FUNC_BUFFER)
    free(fmt);

  return done;
}

#define DEF_LOG_FUNC(name, file, color) \
  int (name)(const char *module, const char *format, ...) {\
    int done; \
    va_list arg; \
    if(flag_be_quiet == 1) \
      return 0; /* number of bytes written is zero */ \
    va_start(arg, format); \
    done = LOG((file), (color), module, format, arg); \
    va_end(arg); \
    return done; \
  }
DEF_LOG_FUNC(log_info, stdout, ANSI_COLOR_BLUE)
DEF_LOG_FUNC(log_failure, stderr, ANSI_COLOR_RED)
DEF_LOG_FUNC(log_success, stdout, ANSI_COLOR_GREEN)
#undef DEF_LOG_FUNC
