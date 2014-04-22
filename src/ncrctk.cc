
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

#define HALFDIM(x) ((x)-((x)%2))/2
#define FORTHDIM(x) HALFDIM((HALFDIM((x))))
#define WCENTERFS(win, str, y) (win)->GotoXY(((win)->GetWidth()-(win)->Length((str)))/2, y);
#define WCENTERFSP(win, str, y) WCENTERFS((win),(str),(y)); *(win)<<(str);

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
    nc::Window *about;
    nc::Window *log;
  }

  void w_create_main(void)
  {
    win::main = new nc::Window(0, 0, COLS, LINES, "", nc::clWhite, nc::brWhite);
    win::main->SetTitle(PACKAGE);
  }

  void w_create_about(void)
  {
    win::about = new nc::Window(FORTHDIM(COLS), FORTHDIM(LINES), HALFDIM(COLS), 10, "", nc::clWhite, nc::brWhite);
    win::about->SetTitle("About");

    WCENTERFS(win::about, "ncrctk "VERSION, 0);
    *win::about << nc::fmtBold << nc::Colors(nc::clGreen, nc::clDefault) << "ncrctk " << nc::fmtBoldEnd
      << nc::Colors(nc::clDefault, nc::clDefault) << VERSION;

    WCENTERFSP(win::about, "a ncurses interface to the crcTk", 1);
    WCENTERFSP(win::about, "Copyright (C) 2014 Jens Oliver John <"PACKAGE_BUGREPORT">", 3);
    WCENTERFSP(win::about, "Licensed under the GNU General Public License v3 or later", 4);
    WCENTERFSP(win::about, "Project homepage: https://github.com/2ion/crctk", 5);
  }

  void do_about(void)
  {
    win::about->Display();
    win::about->ReadKey();
    win::about->Hide();
  }

  void init(void)
  {
    nc::InitScreen(PACKAGE, true);
    w_create_main();
    w_create_about();
    win::main->Display();
  }

  void deinit(void)
  {
    delete win::about;
    delete win::main;
    nc::DestroyScreen();
  }
}

int main(int argc, char **argv)
{
  ui::init();
  ui::do_about();
  ui::win::about->ReadKey();
  ui::deinit();
  return 0;
}
