#pragma once

#include <enum/enum.h>
#include <functional>
#include <unordered_map>

#include "inc/essential.h"
#include "util/buffer.h"
#include "util/bytes.h"
#include "util/endian.h"

class Context;

class Contract
{
public:
  virtual ~Contract() {}
  virtual std::unique_ptr<Contract> clone() const = 0;

  virtual void debug_create() const = 0;
  virtual void debug_save() const = 0;

  void call_buf(Buffer& buf)
  {
    auto func_id = buf.read<uint16_t>();
    if (auto it = functions.find(func_id); it != functions.end()) {
      (it->second)(this, buf);
    } else {
      throw Error("Invalid function ident");
    }
  }

  template <typename T>
  T* as()
  {
    T* result = dynamic_cast<T*>(this);
    if (result == nullptr)
      throw Error("Invalid contract cast");

    return result;
  }

protected:
  Contract(const Address& addr)
      : m_addr(addr)
  {
  }

  /// Get the sender of the current message. Throws if the sender has not
  /// been set.
  Address get_sender();

  /// Set the sender of the current message to this contract.
  void set_sender();

  /// TODO: Move this to static level
  template <typename T, typename... Args>
  void add_callable(const uint16_t func_id, void (T::*func)(Args...))
  {
    functions.emplace(func_id, [func](void* obj, Buffer& buf) {
      std::tuple<T*, Args...> tup{(T*)obj, buf.read<Args>()...};
      std::function<void(T*, Args...)> func_cast = func;
      std::apply(func_cast, tup);
    });
  }

public:
  const Address m_addr;

protected:
  std::unordered_map<uint16_t, std::function<void(void*, Buffer&)>> functions;
};
