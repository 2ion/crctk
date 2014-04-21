
#define _XOPEN_SOURCE_EXTENDED

#include <locale.h>
#include <stdlib.h>
#include <cwidget/toplevel.h>
#include <cwidget/dialogs.h>
#include <clocale>

extern "C" {
#include "util.h"
#include "database.h"
}

namespace cw = cwidget;

const char *crcregex = "[[:xdigit:]]\\{8\\}";
const char *crcregex_stripper = "[[:punct:]]\\?[[:xdigit:]]\\{8\\}[[:punct:]]\\?";
const char *dbiofile = "crcsums.tdb";
const char *hexarg = "00000000";

int main(int argc, char **argv) {
  cw::toplevel::init();
  cw::widgets::widget_ref dialog = cw::dialogs::ok(L"おはよう世界！", cw::util::arg(sigc::ptr_fun(cw::toplevel::exitmain)));
  cw::toplevel::settoplevel(dialog);
  cw::toplevel::mainloop();
  cw::toplevel::shutdown();
  return 0;
}
