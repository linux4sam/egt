//
// detail/winrt_utils.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EGT_ASIO_DETAIL_WINRT_UTILS_HPP
#define EGT_ASIO_DETAIL_WINRT_UTILS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <egt/asio/detail/config.hpp>

#if defined(EGT_ASIO_WINDOWS_RUNTIME)

#include <codecvt>
#include <cstdlib>
#include <future>
#include <locale>
#include <robuffer.h>
#include <windows.storage.streams.h>
#include <wrl/implements.h>
#include <egt/asio/buffer.hpp>
#include <egt/asio/error_code.hpp>
#include <egt/asio/detail/memory.hpp>
#include <egt/asio/detail/socket_ops.hpp>

#include <egt/asio/detail/push_options.hpp>

namespace egt {
namespace asio {
namespace detail {
namespace winrt_utils {

inline Platform::String^ string(const char* from)
{
  std::wstring tmp(from, from + std::strlen(from));
  return ref new Platform::String(tmp.c_str());
}

inline Platform::String^ string(const std::string& from)
{
  std::wstring tmp(from.begin(), from.end());
  return ref new Platform::String(tmp.c_str());
}

inline std::string string(Platform::String^ from)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.to_bytes(from->Data());
}

inline Platform::String^ string(unsigned short from)
{
  return string(std::to_string(from));
}

template <typename T>
inline Platform::String^ string(const T& from)
{
  return string(from.to_string());
}

inline int integer(Platform::String^ from)
{
  return _wtoi(from->Data());
}

template <typename T>
inline Windows::Networking::HostName^ host_name(const T& from)
{
  return ref new Windows::Networking::HostName((string)(from));
}

template <typename ConstBufferSequence>
inline Windows::Storage::Streams::IBuffer^ buffer_dup(
    const ConstBufferSequence& buffers)
{
  using Microsoft::WRL::ComPtr;
  using egt::asio::buffer_size;
  std::size_t size = buffer_size(buffers);
  auto b = ref new Windows::Storage::Streams::Buffer(size);
  ComPtr<IInspectable> insp = reinterpret_cast<IInspectable*>(b);
  ComPtr<Windows::Storage::Streams::IBufferByteAccess> bacc;
  insp.As(&bacc);
  byte* bytes = nullptr;
  bacc->Buffer(&bytes);
  egt::asio::buffer_copy(egt::asio::buffer(bytes, size), buffers);
  b->Length = size;
  return b;
}

} // namespace winrt_utils
} // namespace detail
} // namespace asio
} // namespace egt

#include <egt/asio/detail/pop_options.hpp>

#endif // defined(EGT_ASIO_WINDOWS_RUNTIME)

#endif // EGT_ASIO_DETAIL_WINRT_UTILS_HPP
