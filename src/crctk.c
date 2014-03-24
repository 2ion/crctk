/*
 * crctk - CRC32 Hexstring Toolkit
 * Copyright (C) 2014 2ion (asterisk!2ion!de)
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
 * The CRC32 computer is based on code by elektron, formerly available
 * at            
 *      http://www.linuxquestions.org/questi...hreadid=199640 and
 *      http://kremlor.net/projects/crc32
 *
 * Thanks to elektron. 
 * */

#include "crctk.h"
#include "command_calc_batch.h"
#include "command_check_batch.h"
#include "command_help.h"
#include "command_list.h"
#include "command_tag.h"
#include "command_remove_tag.h"
#include "command_check.h"
#include "command_check_hexstring.h"
#include "command_calc.h"
#include "command_idle.h"

const char *crcregex = "[[:xdigit:]]\\{8\\}";
const char *crcregex_stripper =
  "[[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?";
const char *dbiofile = "crcsums.tdb";
const char *hexarg = "00000000";
const char *optstring_short = "+X:xtnvV:hsrC:ce:p:a";
const struct option options_long[] = {
  { "verify", no_argument, NULL, 'v' },
  { "verify-db", required_argument, NULL, 'V' },
  { "prefer-hexstring", no_argument, NULL, 'x' },
  { "calc", no_argument, NULL, 'c' },
  { "numerical", no_argument, NULL, 'n' },
  { "create-db", required_argument, NULL, 'C' },
  { "append", no_argument, NULL, 'a' },
  { "print", required_argument, NULL, 'p' },
  { "tag", no_argument, NULL, 't' },
  { "strip-tag", no_argument, NULL, 's' },
  { "remove-tag", no_argument, NULL, 'r' },
  { "tag-regex", required_argument, NULL, 'e' },
  { "help", no_argument, NULL, 'h' },
  { "version", no_argument, NULL, 'J' },
  { 0, 0, 0, 0 }
};

int main(int argc, char **argv) {
  int opt = 0;
  int cmdflags = 0;
  CommandFunction cmd = command_idle;

  while((opt = getopt_long(argc, argv, optstring_short,
         options_long, NULL)) != -1)
    switch(opt) {
      case 's': cmdflags |= TAG_ALLOW_STRIP; break;
      case 'x': cmdflags |= CHECK_BATCH_PREFER_HEXSTRING; break;
      case 'a': cmdflags |= APPEND_TO_DB; break;
      case 'n': cmdflags |= CALC_PRINT_NUMERICAL; break;
      case 'p': dbiofile = strdup(optarg); cmd = command_list_db; break;
      case 'V': dbiofile = strdup(optarg); cmd = command_check_batch; break;
      case 'e': crcregex_stripper = strdup(optarg); break;
      case 'c': cmd = command_calc; break;
      case 'C': dbiofile = strdup(optarg); cmd = command_calc_batch; break;
      case 'r': cmd = command_remove_tag; break;
      case 'v': cmd = command_check; break;
      case 'X': hexarg = strdup(optarg); cmd = command_check_hexstring; break;
      case 't': cmd = command_tag; break;
      case 'h': cmd = command_help; break;
      case 'J': puts("crctk version: " VERSION "\n" "Compiled on: " __DATE__ " " __TIME__); return EXIT_SUCCESS; break;
      default:  return EXIT_FAILURE;
    } // switch
  if(optind >= argc &&
      cmd != command_check_batch &&
      cmd != command_list_db && 
      cmd != command_help)
    LERROR(EXIT_FAILURE, 0,
            "Too few arguments. Use the -h flag "
            "to view usage information.");
  return cmd(argc, argv, optind, cmdflags);
}
