#include <iostream>
#include <cstdarg>


#include "server.hpp"

int main(int argc, char **argv)
{

  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    server server(io_context, std::atoi(argv[1]));

    io_context.run();
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Exception: " << ex.what() << "\n";
  }

  return EXIT_SUCCESS;
}
