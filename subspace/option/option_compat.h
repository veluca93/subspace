// Copyright 2023 Google LLC
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

#include <optional>

#include "subspace/mem/addressof.h"
#include "subspace/mem/copy.h"
#include "subspace/mem/move.h"
#include "subspace/option/option.h"

// Include this header to use conversions between sus::Option and std::optional.

namespace sus {

template <class T>
constexpr Option<T> Option<T>::from(
    const std::optional<std::remove_reference_t<T>>& s) noexcept
  requires(::sus::mem::Copy<T> && !std::is_reference_v<T>)
{
  if (s.has_value())
    return Option::with(s.value());
  else
    return Option();
}

template <class T>
constexpr Option<T> Option<T>::from(
    std::optional<std::remove_reference_t<T>>&& s) noexcept
  requires(::sus::mem::Move<T> && !std::is_reference_v<T>)
{
  if (s.has_value())
    return Option::with(::sus::move(s).value());
  else
    return Option();
}

template <class T>
template <::sus::construct::Into<T> U>
constexpr Option<T> Option<T>::from(const std::optional<U>& s) noexcept
  requires(!std::is_reference_v<T>)
{
  if (s.has_value())
    return Option::with(::sus::into(s.value()));
  else
    return Option();
}

template <class T>
template <::sus::construct::Into<T> U>
constexpr Option<T> Option<T>::from(std::optional<U>&& s) noexcept
  requires(!std::is_reference_v<T>)
{
  if (s.has_value())
    return Option::with(::sus::into(::sus::move(s).value()));
  else
    return Option();
}

template <class T>
constexpr Option<T>::Option(
    const std::optional<std::remove_reference_t<T>>& s) noexcept
  requires(::sus::mem::Copy<T> && !std::is_reference_v<T>)
    //: Option(FromOptional, s.has_value() ? &s.value() : nullptr) {}
    : Option() {
  if (s.has_value()) insert(s.value());
}

template <class T>
constexpr Option<T>::Option(
    std::optional<std::remove_reference_t<T>>&& s) noexcept
  requires(::sus::mem::Move<T> && !std::is_reference_v<T>)
    : Option() {
  if (s.has_value()) insert(::sus::move(s).value());
}

template <class T>
constexpr Option<T>::operator std::optional<std::remove_reference_t<T>>()
    const& noexcept
  requires(::sus::mem::Copy<T> && !std::is_reference_v<T>)
{
  if (is_some())
    return std::optional<std::remove_reference_t<T>>(std::in_place, as_value());
  else
    return std::optional<std::remove_reference_t<T>>(std::nullopt);
}

template <class T>
constexpr Option<T>::operator std::optional<
    std::remove_reference_t<T>>() && noexcept
  requires(::sus::mem::Move<T> && !std::is_reference_v<T>)
{
  if (is_some())
    return std::optional<std::remove_reference_t<T>>(
        std::in_place, ::sus::move(*this).unwrap());
  else
    return std::optional<std::remove_reference_t<T>>(std::nullopt);
}

template <class T>
constexpr Option<T>::operator std::optional<const std::remove_reference_t<T>*>()
    const& noexcept
  requires(std::is_reference_v<T>)
{
  if (is_some())
    return std::optional<const std::remove_reference_t<T>*>(
        ::sus::mem::addressof(as_value()));
  else
    return std::optional<std::remove_reference_t<T>*>(std::nullopt);
}

template <class T>
constexpr Option<T>::operator std::optional<std::remove_reference_t<T>*>()
    const& noexcept
  requires(!std::is_const_v<std::remove_reference_t<T>> &&
           std::is_reference_v<T>)
{
  if (is_some()) {
    return std::optional<std::remove_reference_t<T>*>(
        ::sus::mem::addressof(as_value_mut()));
  } else
    return std::optional<std::remove_reference_t<T>*>(std::nullopt);
}

}  // namespace sus
