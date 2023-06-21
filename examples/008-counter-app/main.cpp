#include "app.hpp"

#include <webpp/beast/beast.hpp>

int main() {
    webpp::beast<website::app> app;
    app
      .server               // the server
      .address("127.0.0.1") // listen on localhost
      .port(8080);          // on http port
    return app();
}
