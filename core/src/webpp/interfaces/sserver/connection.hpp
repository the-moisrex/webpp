#include "constants.hpp"

namespace webpp::sserver {


  class connection {
    private:
      socket_t socket;
      std::array<char, buffer_size> buffer;

    public:
      connection(connection const&) = delete;
      connection& operator=(connection const&) = delete;

      /**
       * Start async operations
       */
      void start() noexcept;

      /**
       * We're shutting down everything, kepp up!
       */
      void stop() noexcept;

  };



}
