#pragma once

#include <sodium/crypto_hash_sha256.h>

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/bytes.h"

template <typename T>
inline Hash sha256(gsl::span<T> data)
{
  Hash hash;
  crypto_hash_sha256((unsigned char*)hash.data(), (unsigned char*)data.data(),
                     data.size_bytes());
  return hash;
}

template <int SIZE>
inline Hash sha256(const Bytes<SIZE>& data)
{
  return sha256(data.as_span());
}

template <typename... Args>
inline Hash sha256(Args... args)
{
  Buffer buf;
  add_buffer(buf, args...);
  return sha256(buf.as_span());
}
