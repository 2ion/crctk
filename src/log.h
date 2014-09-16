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

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "crctk.h"

#define ANSI_COLOR_RED      "\x1b[0;31m"
#define ANSI_COLOR_GREEN    "\x1b[0;32m"
#define ANSI_COLOR_YELLOW   "\x1b[0;33m"
#define ANSI_COLOR_BLUE     "\x1b[0;34m"
#define ANSI_COLOR_MAGENTA  "\x1b[0;35m"
#define ANSI_COLOR_CYAN     "\x1b[0;36m"
#define ANSI_COLOR_REDB     "\x1b[1;31m"
#define ANSI_COLOR_GREENB   "\x1b[1;32m"
#define ANSI_COLOR_YELLOWB  "\x1b[1;33m"
#define ANSI_COLOR_BLUEB    "\x1b[1;34m"
#define ANSI_COLOR_MAGENTAB "\x1b[1;35m"
#define ANSI_COLOR_CYANB    "\x1b[1;36m"
#define ANSI_COLOR_RESET    "\x1b[0m"
#define IFCOLORS(str)       (flag_use_colors==1?(str):("")) 

int LOG(const char *module, const char *format, ...);

#endif
