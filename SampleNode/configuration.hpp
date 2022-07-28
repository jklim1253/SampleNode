#pragma once

#include <unordered_map>
#include <string>

namespace sv
{

template<class KT, class VT>
class basic_configuration
{
public:
  using key_type = KT;
  using value_type = VT;
  using self = basic_configuration<key_type, value_type>;

  using container = std::unordered_map<key_type, value_type>;
public:

  static const value_type nvalue;

public:
  basic_configuration()
  {
  }
  ~basic_configuration()
  {
  }

  bool has(key_type const& key) const
  {
    return _depot.find(key) != _depot.end();
  }

  value_type& value(key_type const& key)
  {
    return _depot[key];
  }
  value_type const& value(key_type const& key) const
  {
    auto target = _depot.find(key);
    if (target == _depot.end()) return nvalue;

    return target->second;
  }

  value_type& operator[](key_type const& key)
  {
    return value(key);
  }
  value_type const& operator[](key_type const& key) const
  {
    return value(key);
  }

private:
  container _depot;
};
template<class K, class V>
const typename basic_configuration<K, V>::value_type basic_configuration<K, V>::nvalue;

using configuration = basic_configuration<std::string, std::string>;

} // namespace sv