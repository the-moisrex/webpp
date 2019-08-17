#include <webpp/webpp.h>

int main() {
  using namespace webpp;

  webpp<cgi> app;

  return app.run();
}
