#pragma once

#include <iostream>
#include <memory>

#include "use_asio.hpp"

namespace sv
{
namespace net
{

template<class PTC>
class basic_session
{
public:
  using protocol_type = PTC;
  using self = basic_session<protocol_type>;
  using ptr = std::shared_ptr<self>;

public:
  basic_session(asio::io_context& _ioc)
    : r_ioc(_ioc)
    , _socket(r_ioc)
    , _readbuffer()
    , _writebuffer()
    , _readstream(&_readbuffer)
    , _writestream(&_writebuffer)
  {
  }
  virtual ~basic_session()
  {
    stop();
  }

  tcp::socket& socket()
  {
    return _socket;
  }
  tcp::socket const& socket() const
  {
    return _socket;
  }

  void start()
  {
    asio::dispatch(r_ioc, std::bind(&self::read, this));
  }
  void stop()
  {
    if (_socket.is_open())
    {
      _socket.close();
    }
  }
private:
  void on_error(error_code const& ec, const char* tag)
  {
    std::stringstream ss;
    ss << '[' << tag << "][" << ec.value() << "] " << ec.message() << '\n';
    std::cout << ss.str() << std::flush;
  }
  void read()
  {
    asio::dispatch(r_ioc, std::bind(&self::do_read, this));
  }
  std::size_t on_read_condition(error_code const& ec, std::size_t bytes, std::size_t total)
  {
    if (ec)
    {
      on_error(ec, "on_read_condition");
      return 0ULL;
    }

    return total - bytes;
  }
  void do_read()
  {
    asio::dispatch(r_ioc, std::bind(&do_read_header, this));
  }
  void do_read_header()
  {
    std::size_t total = protocol_type::get_header_size();
    asio::async_read(_socket,
                     _readbuffer,
                     std::bind(&on_read_condition, this, _1, _2, total),
                     std::bind(&on_read_header, this, _1, _2));
  }
  void on_read_header(error_code const& ec, std::size_t bytes)
  {
    if (ec)
    {
      on_error(ec, "on_read_header");
      return;
    }

    // read header from stream
    std::size_t length;
    _readstream.read((char*)&length, sizeof(std::size_t));

    asio::dispatch(r_ioc, std::bind(&self::do_read_body, this, length));
  }
  void do_read_body(std::size_t length)
  {
    asio::async_read(_socket,
                     _readbuffer,
                     std::bind(&on_read_condition, this, _1, _2, length),
                     std::bind(&on_read_body, this, _1, _2));
  }
  void on_read_body(error_code const& ec, std::size_t bytes)
  {
    if (ec)
    {
      on_error(ec, "on_read_body");
      return;
    }

    // read body from stream.
  }

private:
  asio::io_context& r_ioc;
  tcp::socket _socket;

  asio::streambuf _readbuffer;
  asio::streambuf _writebuffer;

  std::istream _readstream;
  std::ostream _writestream;
};


} // namespace sv::net
} // namespace sv