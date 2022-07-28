#pragma once

#include <cstdint>
#include <string>
#include <iostream>

namespace sv
{
namespace net
{

template<class IMPL>
class basic_protocol
{
public:
  using self = basic_protocol<IMPL>;
  using value_type = typename IMPL::value_type;
  using packet_type = typename IMPL::packet_type;

public:
  std::size_t get_header_size() const
  {
    return IMPL::get_header_size();
  }
  std::istream& read_header(std::istream& is)
  {
    return IMPL::read_header(is, packet);
  }
  std::size_t get_body_size() const
  {
    return IMPL::get_body_size(packet);
  }
  std::istream& read_body(std::istream& is)
  {
    return IMPL::read_body(is, packet);
  }
  value_type get_body() const
  {
    return IMPL::get_body(packet);
  }

private:
  packet_type packet;
};

namespace protocol_base
{

struct singlestring
{
  using value_type = std::string;
  using packet_type = singlestring;

#pragma pack(push, 1)
  struct header_t
  {
    std::uint16_t magic;
    std::uint32_t size;
    std::uint32_t command;
  } header;
#pragma pack(pop)

  struct body_t
  {
    std::string data;
  } body;

  static std::size_t get_header_size()
  {
    return sizeof(header_t);
  }
  static std::istream& read_header(std::istream& is, packet_type& packet)
  {
    is.read((char*)&packet.header, sizeof(header_t));
    return is;
  }
  static std::size_t get_body_size(packet_type const& packet)
  {
    return packet.header.size;
  }
  static std::istream& read_body(std::istream& is, packet_type& packet)
  {
    packet.body.data.resize(packet.header.size);
    is.read((char*)&packet.body.data[0], packet.header.size);
    return is;
  }
  static value_type get_body(packet_type const& packet)
  {
    return packet.body.data;
  }
};

} // namespace sv::net::protocol_base
} // namespace sv::net
} // namespace sv