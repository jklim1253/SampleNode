#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif // _WIN32_WINNT
#include <boost/asio.hpp>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

#include <functional>
#include <chrono>

using namespace std::placeholders;
using namespace std::literals::chrono_literals;
