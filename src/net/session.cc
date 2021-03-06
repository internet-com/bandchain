#include "session.h"

Session::Session(boost::asio::ip::tcp::socket _socket, NetApplication& _app)
    : app(_app)
    , socket(std::move(_socket))
{
}

void Session::serve() { async_read(); }

void Session::process_read(size_t length)
{
  read_buffer << gsl::span(socket_buffer, length);
  while (app.process(read_buffer, write_buffer)) {
    // Keep processing incoming messages until everything is done.
  }
  async_write();
}

void Session::process_write(size_t length)
{
  write_buffer.consume(length);
  async_read();
}

void Session::async_read()
{
  boost::asio::async_read(
      socket, boost::asio::buffer(socket_buffer, sizeof(socket_buffer)),
      boost::asio::transfer_at_least(1),
      [self = shared_from_this()](const auto ec, size_t length) {
        if (!ec) {
          self->process_read(length);
        } else {
          WARN(log, "Failed to read from socket. Client disconnected");
        }
      });
}

void Session::async_write()
{
  if (write_buffer.empty()) {
    return async_read();
  }

  auto write_span = write_buffer.as_span();
  boost::asio::async_write(
      socket, boost::asio::buffer(write_span.data(), write_span.size_bytes()),
      [self = shared_from_this()](const auto ec, size_t length) {
        if (!ec) {
          self->process_write(length);
        } else {
          WARN(log, "Failed to write to socket. Client disconnected");
        }
      });
}
