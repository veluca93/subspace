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

#include <functional>

#include "marker/unsafe.h"
#include "option/option.h"

namespace sus::traits::iter {

using ::sus::option::Option;

template <class Item>
class IteratorLoop;
struct IteratorEnd;

template <class Item>
class Iterator {
 public:
  Iterator(Iterator&&) = default;
  Iterator& operator=(Iterator&&) = default;

  // Required methods.

  /// Gets the next element from the iterator, if there is one. Otherwise, it
  /// returns an Option holding #None.
  virtual Option<Item> next() noexcept = 0;

  // Provided methods.

  IteratorLoop<Item> begin() & noexcept;
  IteratorEnd end() & noexcept;

  /// Tests whether all elements of the iterator match a predicate.
  ///
  /// If the predicate returns `true` for all elements in the iterator, this
  /// functions returns `true`, otherwise `false`. The function is
  /// short-circuiting; it stops iterating on the first `false` returned from
  /// the predicate.
  ///
  /// Returns `true` if the iterator is empty.
  virtual bool all(std::function<bool(Item)> f) noexcept;

  /// Tests whether any elements of the iterator match a predicate.
  ///
  /// If the predicate returns `true` for any elements in the iterator, this
  /// functions returns `true`, otherwise `false`. The function is
  /// short-circuiting; it stops iterating on the first `true` returned from the
  /// predicate.
  ///
  /// Returns `false` if the iterator is empty.
  virtual bool any(std::function<bool(Item)> f) noexcept;

  /// Consumes the iterator, and returns the number of elements that were in it.
  ///
  /// The function walks the iterator until it sees an Option holding #None.
  ///
  /// # Safety
  ///
  /// If the `usize` type does not have trapping arithmetic enabled, and the
  /// iterator has more than `usize::MAX` elements in it, the value will wrap
  /// and be incorrect. Otherwise, `usize` will catch overflow and panic.
  virtual /* TODO: usize */ size_t count() noexcept;

 protected:
  Iterator() = default;
};

}  // namespace sus::traits::iter
