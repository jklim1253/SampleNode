#pragma once

#include <memory>
#include <thread>
#include <sstream>
#include <list>
#include <iostream>

#include "use_asio.hpp"
#include "configuration.hpp"

namespace sv
{
namespace net
{

template<class ST>
class basic_server
{
public:
  using session_type = ST;
  using session_ptr = typename session_type::ptr;
  using self = basic_server<session_type>;
  using ptr = std::shared_ptr<self>;

  template<class...Args>
  static ptr make(Args&&...args)
  {
    return std::make_shared<self>(std::forward<Args>(args)...);
  }

public:
  basic_server()
    : _ioc()
    , _work_guard(_ioc.get_executor())
    , _worker([&] { _ioc.run(); })
    , _acceptor(_ioc)
  {

  }
  virtual ~basic_server()
  {
    stop();
  }

  void start(configuration config)
  {
    if (!config.has("port"))
    {
      throw std::exception("basic_server::start() invalid config");
    }

    auto port = static_cast<unsigned short>(std::stoi(config["port"]));

    tcp::endpoint ep(tcp::v4(), port);

    _acceptor.open(ep.protocol());
    _acceptor.bind(ep);
    _acceptor.set_option(tcp::acceptor::reuse_address(true));

    _acceptor.listen();

    asio::post(_ioc, std::bind(&self::do_accept, this));
  }
  void stop()
  {
    _work_guard.reset();
    _worker.join();
  }

private:
  void on_error(error_code const& ec, const char* tag)
  {
    std::stringstream ss;
    ss << '[' << tag << "][" << ec.value() << "] " << ec.message() << '\n';
    std::cout << ss.str() << std::flush;
  }
  void do_accept()
  {
    auto session = session_type::make(_ioc);
    _acceptor.async_accept(session->socket(),
                           std::bind(&self::on_accept, this, _1, session));
  }
  void on_accept(error_code const& ec, session_ptr session)
  {
    if (ec)
    {
      on_error(ec, "on_accept");
      return;
    }

    asio::post(_ioc, std::bind(&self::do_accept, this));

    std::stringstream ss;
    ss << '[' << session->socket().remote_endpoint() << "] is accepted\n";
    std::cout << ss.str() << std::flush;

    _sessionlist.push_back(session);

    asio::post(_ioc, std::bind(&self::start, session));
  }

private:
  asio::io_context _ioc;
  asio::executor_work_guard<asio::io_context::executor_type> _work_guard;
  std::thread _worker;

  tcp::acceptor _acceptor;

  std::list<session_ptr> _sessionlist;
};

} // namespace sv::net
} // namespace sv