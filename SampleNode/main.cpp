#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>

#include "server.hpp"

template<class T>
class test_if
{
public:
	using value_type = typename T::value_type;
	using packet_type = typename T::packet_type;

public:
	// read part
	std::size_t get_header_size() const
	{
		return T::get_header_size();
	}
	std::istream& read_header(std::istream& is)
	{
		return T::read_header(is, packet);
	}
	std::size_t get_body_size() const
	{
		return T::get_body_size(packet);
	}
	std::istream& read_body(std::istream& is)
	{
		return T::read_body(is, packet);
	}
	value_type get_body() const
	{
		return T::get_body(packet);
	}

	// write part

private:
	packet_type packet;
};

struct sample_t
{
#pragma pack(push, 1)
	// header
	struct header_t
	{
		std::uint16_t magic;
		std::uint32_t size;
		std::uint32_t command;
	} header;
#pragma pack(pop)

	// body
	struct body_t
	{
    std::string data;
	} body;
};

class sample : sample_t
{
public:
	using value_type = std::string;
	using packet_type = sample_t;

public:
	static std::size_t get_header_size()
	{
		return sizeof(sample_t::header_t);
	}
	static std::istream& read_header(std::istream& is, packet_type& packet)
	{
		is.read((char*)&packet.header, sizeof(packet.header));
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

int main()
{
	try
	{
		// prepare sample dump.
		sample_t dump
		{
			{
				0xFFEF,
				(std::uint32_t)strlen("hello world"),
				0x00000001
      },
			"hello world"
		};
		std::stringstream ss;
		ss.write((const char*)&dump.header, sizeof(dump.header));
		ss.write(dump.body.data.c_str(), dump.body.data.length());

		// read
		// read header
		auto ptc = new test_if<sample>();
		std::cout << "header size: " << ptc->get_header_size() << std::endl;

		ptc->read_header(ss);

		// read body
		std::cout << "body size: " << ptc->get_body_size() << std::endl;

		ptc->read_body(ss);

		std::cout << "body content: " << ptc->get_body() << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "exception: " << ex.what() << std::endl;
	}

  return 0;
}