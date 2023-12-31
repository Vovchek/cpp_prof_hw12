
#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

#include "processor.hpp"

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
 public:
  session(tcp::socket socket) : socket_(std::move(socket)) {
    cmd = &ICommand<Net2SQL>::getInstance();
  }

  void start() { do_read(); }

 private:
  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
          if (!ec) {
            length = cmd->process(data_, length);
            do_write(length);
          }
        });
  }

  void do_write(std::size_t length) {
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_, boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            do_read();
          }
        });
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  ICommand<Net2SQL> *cmd;
};

class server {
 public:
  server(boost::asio::io_context& io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
  }

 private:
  void do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            std::make_shared<session>(std::move(socket))->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
};
