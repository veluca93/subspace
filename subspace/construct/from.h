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

#include <concepts>

namespace sus::construct {

/// A concept that indicates ToType can be constructed from a FromType, via
/// `ToType::from(FromType)`.
///
/// This concept is rarely used directly, instead prefer to use the
/// `sus::construct::Into` concept.
///
/// When a type is `From<T, O>`, it is also `Into<O, T>`. Then a variable `o` of
/// type `O` can be explicitly converted to `T`, with type deduction, via
/// `sus::into(o)`.
template <class ToType, class FromType>
// clang-format off
concept From = requires (FromType&& from) {
    { ToType::from(static_cast<FromType&&>(from)) } -> std::same_as<ToType>;
};
// clang-format on

}  // namespace sus::construct
