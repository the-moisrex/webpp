#include <utility>



class session {
  public:
    using socket_t = boost::asio::ip::tcp::socket;

  private:
    socket_t _socket;

    /**
     * This function will read the data and pass it to read_callback to handle those data
     */
    void read() {
      
    }

    void read_callback(boost::system::error_code const&, std::size_t length) noexcept;

    /**
     * This method will write the data and calls the write_callback to handle the next steps
     */
    void write();
    void write_callback(boost::system::error_code const&, std::size_t length) noexcept;

  public:
    session(socket_t&& _socket) noexcept : _socket(std::move(_socket)) {}

};


