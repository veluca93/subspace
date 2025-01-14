// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <string.h>

#include <type_traits>

#include "marker/unsafe.h"
#include "mem/addressof.h"
#include "mem/move.h"
#include "mem/mref.h"
#include "mem/relocate.h"
#include "mem/size_of.h"

namespace sus::mem {

// It would be nice to have an array overload of replace() but functions can't
// return arrays.

template <class T>
  requires(!std::is_array_v<T> && std::is_move_constructible_v<T> &&
           std::is_copy_assignable_v<T>)
[[nodiscard]] inline constexpr T replace(T& dest, const T& src) noexcept {
  auto old = T(::sus::move(dest));

  // memcpy() is not constexpr so we can't use it in constexpr evaluation.
  bool can_memcpy =
      ::sus::mem::relocate_one_by_memcpy<T> && !std::is_constant_evaluated();
  if (can_memcpy) {
    memcpy(::sus::mem::addressof(dest), ::sus::mem::addressof(src),
           ::sus::mem::data_size_of<T>());
  } else {
    dest = src;
  }

  return old;
}

template <class T>
  requires(!std::is_array_v<T> && std::is_move_constructible_v<T> &&
           std::is_move_assignable_v<T>)
[[nodiscard]] inline constexpr T replace(T& dest, T&& src) noexcept {
  auto old = T(::sus::move(dest));

  // memcpy() is not constexpr so we can't use it in constexpr evaluation.
  bool can_memcpy =
      ::sus::mem::relocate_one_by_memcpy<T> && !std::is_constant_evaluated();
  if (can_memcpy) {
    memcpy(::sus::mem::addressof(dest), ::sus::mem::addressof(src),
           ::sus::mem::data_size_of<T>());
  } else {
    dest = ::sus::move(src);
  }

  return old;
}

template <class T>
  requires(!std::is_array_v<T> && std::is_copy_assignable_v<T>)
inline void replace_and_discard(T& dest, const T& src) noexcept {
  // memcpy() is not constexpr so we can't use it in constexpr evaluation.
  bool can_memcpy =
      ::sus::mem::relocate_one_by_memcpy<T> && !std::is_constant_evaluated();
  if (can_memcpy) {
    memcpy(::sus::mem::addressof(dest), ::sus::mem::addressof(src),
           ::sus::mem::data_size_of<T>());
  } else {
    dest = src;
  }
}

template <class T>
  requires(!std::is_array_v<T> && std::is_move_assignable_v<T>)
inline void replace_and_discard(T& dest, T&& src) noexcept {
  // memcpy() is not constexpr so we can't use it in constexpr evaluation.
  bool can_memcpy =
      ::sus::mem::relocate_one_by_memcpy<T> && !std::is_constant_evaluated();
  if (can_memcpy) {
    memcpy(::sus::mem::addressof(dest), ::sus::mem::addressof(src),
           ::sus::mem::data_size_of<T>());
  } else {
    dest = ::sus::move(src);
  }
}

template <class T>
[[nodiscard]] inline constexpr T* replace_ptr(T*& dest, T* src) noexcept {
  T* old = dest;
  dest = src;
  return old;
}

template <class T>
[[nodiscard]] inline constexpr T* replace_ptr(T*& dest,
                                              decltype(nullptr)) noexcept {
  T* old = dest;
  dest = nullptr;
  return old;
}


}  // namespace sus::mem
