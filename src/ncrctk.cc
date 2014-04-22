
#define _XOPEN_SOURCE_EXTENDED

#include <locale.h>
#include <stdlib.h>
#include <clocale>
#include <string>

extern "C"
{
#include "util.h"
#include "database.h"
}
#include "localerror.h"
#include "window.h"

#include "config.h"

/* globals */

const char *crcregex = "[[:xdigit:]]\\{8\\}";
const char *crcregex_stripper = "[[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?";
const char *dbiofile = "crcsums.tdb";
const char *hexarg = "00000000";

namespace nc = NCurses;

namespace ui
{
  namespace win
  {
    nc::Window *main;
    nc::Window *log;
  }
}

int main(int argc, char **argv)
{
  nc::InitScreen(PACKAGE, true);
  ui::win::main = new nc::Window(0, 0, COLS, LINES, "", nc::clWhite, nc::brWhite);

  *ui::win::main << nc::clWhite;
  *ui::win::main << "Hello World!";

  ui::win::main->Display();
  ui::win::main->Refresh();

  sleep(5);

  delete ui::win::main;

  nc::DestroyScreen();
  return 0;
}
