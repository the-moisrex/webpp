#include "app.hpp"

int main() {
    webpp::beast<website::app> server;
    server
      .address("127.0.0.1") // listen on localhost
      .port(8080);          // on http port
    return server();
}
