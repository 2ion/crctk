
#define _XOPEN_SOURCE_EXTENDED

#include <stdlib.h>
#include <clocale>
#include <csignal>
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

extern "C"
{
const char *crcregex = "[[:xdigit:]]\\{8\\}";
const char *crcregex_stripper = "[[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?";
const char *dbiofile = "crcsums.tdb";
const char *hexarg = "00000000";
}

namespace nc = NCurses;

namespace global
{
  std::string db;
}

/* the rest */

namespace ui
{
  /* prototypes */

  void w_create_main(void);
  void w_create_about(void);
  void w_switchto(nc::Window*,bool);
  void do_about(void);
  void init(void);
  void deinit(void);
  void sighandler_resize(int);

   /* definitions */

  namespace win
  {
    nc::Window *active;

    nc::Window *main;
    nc::Window *about;
    nc::Window *log;
  }

  namespace menu
  {
  
  }
 
  void w_create_main(void)
  {
    win::main = new nc::Window(0, 0, COLS, LINES, "", nc::clWhite, nc::brWhite);
    win::main->SetTitle("ncrctk");
  }

  void w_create_about(void)
  {
    win::about = new nc::Window(FORTHDIM(COLS), FORTHDIM(LINES), HALFDIM(COLS), 10, "", nc::clWhite, nc::brWhite);
    win::about->SetTitle("About");

    WCENTERFS(win::about, "ncrctk "VERSION, 0);
    *win::about << nc::fmtBold << nc::Colors(nc::clGreen, nc::clDefault) << "ncrctk " << nc::fmtBoldEnd
      << nc::Colors(nc::clDefault, nc::clDefault) << VERSION;

    WCENTERFSP(win::about, "a ncurses interface to crctk", 1);
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

  void deinit(void)
  {
    delete win::about;
    delete win::main;
    nc::DestroyScreen();
  }

  void init(void)
  {
    nc::InitScreen(PACKAGE, true);
    signal(SIGWINCH, sighandler_resize);
    w_create_main();
    w_create_about();
    win::main->Display();
    win::active = win::main;
  }

  void sighandler_resize(int sig)
  {
    /* indicates that the terminal's got resized */
    if(sig == SIGWINCH)
    {
      ui::win::about->Clear();
      ui::win::main->Clear();

      ui::win::about->Resize(HALFDIM(COLS), 10);
      ui::win::about->Display();

      ui::win::main->Resize(COLS, LINES);
      ui::win::main->Display();
    }
  }

  void w_switchto(nc::Window *w, bool is_overlay = false)
  {
    if(w == win::active)
      return;
    if(is_overlay)
      win::active->Hide();
    win::active = w;
    win::active->Display();
  }
}


int main(int argc, char **argv)
{
  int input = 0;


  ui::init();

  while(1)
  {
    ui::win::active->ReadKey(input);
    if(ui::win::active == ui::win::about)
    {
      ui::w_switchto(ui::win::main);
    } // win::about
    else if(ui::win::active == ui::win::main)
    {
      if(input == 'q')
        break;
      if(input == '?')
      {
        ui::w_switchto(ui::win::about);
      }
    } // win::main
  }

  ui::deinit();
  return 0;
}
