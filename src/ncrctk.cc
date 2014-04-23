
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
#define SUBWINCOLS (COLS-2)
#define SUBWINLINES (LINES-4)
#define SUBWINOCOL 1
#define SUBWINOLINE 3

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
  void w_create_inspector(void);
  void w_create_error(void);
  void w_switchto(nc::Window*,bool);
  std::string do_prompt(nc::Window*, const int * coords);
  void do_about(void);
  void do_error(std::string &msg);
  void init(void);
  void deinit(void);
  void sighandler_resize(int);

   /* definitions */

  namespace win
  {
    nc::Window *active;

    nc::Window *main;
    nc::Window *about;
    nc::Window *inspector;
    nc::Window *error;
    nc::Window *log;

    namespace res
    {
      namespace inspector
      {
        int dbprompt[2];
        std::string dbpath;
        struct DBItem dbi = DBITEM_NULL;
      }
    }
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

  void w_create_error(void)
  {
    win::error = new nc::Window(FORTHDIM(COLS), FORTHDIM(LINES),
        HALFDIM(COLS), 4, "", nc::clWhite, nc::brRed);
    *win::error << "No error yet";
  }

  void w_create_inspector(void)
  {
    win::inspector = new nc::Window(SUBWINOCOL, SUBWINOLINE, SUBWINCOLS, SUBWINLINES, "", nc::clWhite, nc::brNone);
    win::inspector->SetTitle("Inspector");

    *win::inspector << nc::fmtBold << "Database: " << nc::fmtBoldEnd;
    win::res::inspector::dbprompt[0] = 0 + win::inspector->Length("Database: ");
    win::res::inspector::dbprompt[1] = 0;
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

    w_create_about();
    w_create_inspector();
    w_create_main();

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

  std::string do_prompt(nc::Window *w, const int * coords)
  {
    w->GotoXY(coords[0], coords[1]);
    return w->GetString(-1, w->GetWidth()-coords[0]);
  }

  void do_error(const std::string &msg)
  {
    win::error->Clear();
    *win::error << msg;
    win::error->Display();
    win::error->ReadKey();
    win::error->Hide();
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
    else if(ui::win::active == ui::win::inspector)
    {
      if(input == 'g')
      {
        std::string i = ui::do_prompt(ui::win::active, ui::win::res::inspector::dbprompt);
        if(!i.empty() &&
            check_access_flags_v(i.c_str(), F_OK | R_OK | W_OK, 1) == 0)
        {
          if(DB_read(i.c_str(), &ui::win::res::inspector::dbi) != 0)
          {
            ui::do_error("The specified file is invalid or inaccessible.");
          } else
          {
            ui::do_error("It worked!");
          }
        }
      } else if(input == 'q')
      {
        ui::w_switchto(ui::win::main);
      }
    }
    else if(ui::win::active == ui::win::main)
    {
      if(input == 'q')
        break;
      if(input == '?')
      {
        ui::w_switchto(ui::win::about);
      } else if(input == 'i')
      {
        ui::w_switchto(ui::win::inspector);
      }
    } // win::main
  }

  ui::deinit();
  return 0;
}
