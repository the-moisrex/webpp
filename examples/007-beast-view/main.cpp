#include "app.hpp"

#include <webpp/http/protocol/beast.hpp>

int main() {
    webpp::http::beast<website::app> app;
    app
      .enable_sync()        // call the app in a thread-safe manner
      .address("127.0.0.1") // listen on localhost
      .port(8080);          // on http port
    return app();
}
