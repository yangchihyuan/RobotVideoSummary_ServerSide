#include <cstdlib>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <thread>
#include <iostream>     //without this, std::cout is unknown

using namespace std::chrono;
using boost::asio::ip::tcp;


class server
{
  public:
    server(boost::asio::io_service &io_service, short port_number);

  private:
    void do_accept();
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    short port_number;
};

void receive_socket(short port_number);
void report_results(short port_number);
