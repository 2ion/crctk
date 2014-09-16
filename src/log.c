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

int LOG(const char *module, const char *format, ...) {
  va_list arg;
  int done;
  char *f;
  int msize;

  va_start(arg, format);

  msize = snprintf(NULL, 0, "[%s%s%s] %s\n\n", IFCOLORS(ANSI_COLOR_BLUE),
      module, IFCOLORS(ANSI_COLOR_RESET), format);
  if(msize>sizeof(LOG_FUNC_BUFFER)) {
    f = malloc(msize);
    if(f == NULL)
      LERROR(EXIT_FAILURE, errno, "malloc() failed");
  } else
    f = LOG_FUNC_BUFFER;
  snprintf(f, msize, "[%s%s%s] %s\n\n", IFCOLORS(ANSI_COLOR_BLUE),
      module, IFCOLORS(ANSI_COLOR_RESET), format);

  done = vfprintf(stdout, f, arg);

  if(f != LOG_FUNC_BUFFER)
    free(f);

  va_end(arg);

  return done;
}
