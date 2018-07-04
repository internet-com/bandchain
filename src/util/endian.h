#pragma once

#include <boost/endian/buffers.hpp>
#include <type_traits>

#include "inc/essential.h"
#include "util/buffer.h"

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
Buffer& operator<<(Buffer& buf, T val)
{
  return buf << static_cast<std::underlying_type_t<T>>(val);
}

template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
Buffer& operator>>(Buffer& buf, T& val)
{
  std::underlying_type_t<T> _val;
  buf >> _val;
  val = T(_val);
  return buf;
}

inline Buffer& operator<<(Buffer& buf, uint8_t val)
{
  const boost::endian::big_uint8_buf_t big_endian_val(val);
  return buf << gsl::make_span(&big_endian_val, 1);
}

inline Buffer& operator>>(Buffer& buf, uint8_t& val)
{
  boost::endian::big_uint8_buf_t big_endian_val;
  buf >> gsl::make_span(&big_endian_val, 1);
  val = big_endian_val.value();
  return buf;
}

template <typename T>
void varint_encode(Buffer& buf, T val)
{
  while (true) {
    std::byte towrite{uint8_t(val & 0x7F)};
    val >>= 7;
    if (val != 0) {
      buf << (towrite | std::byte{0x80});
    } else {
      buf << towrite;
      break;
    }
  }
}

template <typename T>
void varint_decode(Buffer& buf, T& val)
{
  val = 0;
  auto buf_span = buf.as_span();

  for (size_t i = 0; i < buf_span.size(); ++i) {
    int byte = std::to_integer<int>(buf_span[i]);
    val |= T(byte & 0x7F) << (7 * i);
    if (!(byte & 0x80)) {
      buf.consume(1 + i);
      return;
    }
  }
  throw Error("Invalid varint decode");
}

inline Buffer& operator<<(Buffer& buf, uint16_t val)
{
  varint_encode(buf, val);
  return buf;
}

inline Buffer& operator<<(Buffer& buf, uint32_t val)
{
  varint_encode(buf, val);
  return buf;
}

inline Buffer& operator<<(Buffer& buf, uint64_t val)
{
  varint_encode(buf, val);
  return buf;
}

inline Buffer& operator<<(Buffer& buf, const uint256_t& val)
{
  varint_encode(buf, val);
  return buf;
}

inline Buffer& operator>>(Buffer& buf, uint16_t& val)
{
  varint_decode(buf, val);
  return buf;
}

inline Buffer& operator>>(Buffer& buf, uint32_t& val)
{
  varint_decode(buf, val);
  return buf;
}

inline Buffer& operator>>(Buffer& buf, uint64_t& val)
{
  varint_decode(buf, val);
  return buf;
}

inline Buffer& operator>>(Buffer& buf, uint256_t& val)
{
  varint_decode(buf, val);
  return buf;
}
