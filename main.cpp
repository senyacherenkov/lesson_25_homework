#include <string>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "async.h"

using boost::asio::ip::tcp;

class session
  : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket, size_t N)
    : socket_(std::move(socket)),
      m_N(N)
  {
  }

  void start()
  {
    async::connect(m_N);
    do_read();
  }

private:
  void do_read()
  {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            async::receive(m_context, data_, length);
            memset(data_, 0, max_length);

            do_read();
          }
        });
  }

  tcp::socket socket_;
  enum { max_length = 10 };
  char data_[max_length];
  size_t m_N = 0;
  handle_t m_context = 0;
};

class server
{
public:
  server(boost::asio::io_context& io_context, unsigned short port, size_t N)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      m_N(N)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<session>(std::move(socket), m_N)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  size_t        m_N;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "first arg is <port>, second is <size of bulk>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    server s(io_context, static_cast<unsigned short>(std::stoi(argv[1])), static_cast<unsigned short>(std::stoi(argv[2])));

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
