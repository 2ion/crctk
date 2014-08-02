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
#include "command_merge.h"
#include "command_delete.h"
#include "command_to_realpaths.h"
#include "util.h"

/* from crctk.h */

const char *crcregex = "[[:xdigit:]]\\{8\\}";
const char *crcregex_stripper =
  "[[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?";
const char *dbiofile = NULL;
const char *hexarg = "00000000";

static const char *optstring_short = "+X:xtnvV:hSsRrC:cd:e:Ppam:";
static const struct option options_long[] = {
  { "verify", no_argument, NULL, 'v' },
  { "verify-db", required_argument, NULL, 'V' },
  { "hexstring", no_argument, NULL, 'x' },
  { "calc", no_argument, NULL, 'c' },
  { "numerical", no_argument, NULL, 'n' },
  { "create-db", required_argument, NULL, 'C' },
  { "delete", required_argument, NULL, 'd' },
  { "append", no_argument, NULL, 'a' },
  { "print", no_argument, NULL, 'p' },
  { "tag", no_argument, NULL, 't' },
  { "strip-tag", no_argument, NULL, 's' },
  { "remove-tag", no_argument, NULL, 'r' },
  { "tag-regex", required_argument, NULL, 'e' },
  { "help", no_argument, NULL, 'h' },
  { "version", no_argument, NULL, 'J' },
  { "merge", required_argument, NULL, 'm' },
  { "realpath", no_argument, NULL, 'R' },
  { "to-realpath", no_argument, NULL, 'P' },
  { "store", no_argument, NULL, 'S' },
  { 0, 0, 0, 0 }
};

int main(int argc, char **argv) {
  int opt = 0;
  int cmdflags = 0;
  int do_free_dbiofile = 0;
  int do_free_hexarg = 0;
  int do_free_crcregexstripper = 0;
  int ret = EXIT_SUCCESS;
  CommandFunction cmd = command_idle;

#define ASSIGN_OPTARG_IF_NULL(cptr, fptr) if((cptr)==NULL){\
  (cptr) = strdup(optarg);\
  (*fptr) = 1;\
}
  while((opt = getopt_long(argc, argv, optstring_short,
         options_long, NULL)) != -1)
    switch(opt) {
      case 's': cmdflags |= TAG_ALLOW_STRIP;
                break;
      case 'x': cmdflags |= CHECK_BATCH_PREFER_HEXSTRING;
                break;
      case 'a': cmdflags |= APPEND_TO_DB;
                break;
      case 'n': cmdflags |= CALC_PRINT_NUMERICAL;
                break;
      case 'R': cmdflags |= USE_REALPATH;
                break;
      case 'p': cmd = command_list_db;
                break;
      case 'S': cmdflags |= USE_REALPATH;
                cmdflags |= APPEND_TO_DB;
                dbiofile = get_capturefilepath();
                do_free_dbiofile = 1;
                break;
      case 'V': ASSIGN_OPTARG_IF_NULL(dbiofile, &do_free_dbiofile);
                cmd = command_check_batch;
                break;
      case 'e': crcregex_stripper = strdup(optarg);
                do_free_crcregexstripper = 1;
                break;
      case 'c': cmd = command_calc;
                break;
      case 'C': ASSIGN_OPTARG_IF_NULL(dbiofile, &do_free_dbiofile);
                cmd = command_calc_batch;
                break;
      case 'P': cmd = command_to_realpaths;
                break;
      case 'r': cmd = command_remove_tag;
                break;
      case 'v': cmd = command_check;
                break;
      case 'X': hexarg = strdup(optarg);
                do_free_hexarg = 1;
                cmd = command_check_hexstring;
                break;
      case 't': cmd = command_tag;
                break;
      case 'h': cmd = command_help;
                break;
      case 'm': dbiofile = strdup(optarg);
                do_free_dbiofile = 1;
                cmd = command_merge;
                break;
      case 'J': printf("crctk version: %s\n", PACKAGE_VERSION);
                printf("Compiled on: %s %s\n", __DATE__, __TIME__);
                return EXIT_SUCCESS;
                break;
      case 'd': dbiofile = strdup(optarg);
                do_free_dbiofile = 1;
                cmd = command_delete;
                break;
      default:  return EXIT_FAILURE;
    } // switch
#undef ASSIGN_OPTARG_IF_NULL
  if(optind >= argc &&
      cmd != command_check_batch &&
      cmd != command_list_db && 
      cmd != command_help) {
    fprintf(stderr, "Too few arguments. Use the -h flag to "
          "obtain usage information.\n");
    return EXIT_FAILURE;
  }
  ret = cmd(argc, argv, optind, cmdflags);
  if(do_free_dbiofile == 1)
    free((void*)dbiofile);
  if(do_free_hexarg == 1)
    free((void*)hexarg);
  if(do_free_crcregexstripper == 1)
    free((void*)crcregex_stripper);
  return ret;
}
