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

#include "fn/fn.h"
#include "iter/__private/iterator_end.h"
#include "iter/__private/iterator_loop.h"
#include "iter/iterator_defn.h"
#include "iter/sized_iterator.h"
#include "mem/move.h"

// IteratorBase provided functions are implemented in this file, so that they
// can be easily included by library users, but don't have to be included in
// every library header that returns an IteratorBase.

namespace sus::iter {

template <class Item>
__private::IteratorLoop<IteratorBase<Item>&>
IteratorBase<Item>::begin() & noexcept {
  return {*this};
}

template <class Item>
__private::IteratorEnd IteratorBase<Item>::end() & noexcept {
  return __private::IteratorEnd();
}

template <class Item>
bool IteratorBase<Item>::all(::sus::fn::FnMut<bool(Item)> f) noexcept {
  while (true) {
    Option<Item> item = next();
    if (item.is_none()) return true;
    // Safety: `item` was checked to hold Some already.
    if (!f(item.take().unwrap_unchecked(::sus::marker::unsafe_fn))) return false;
  }
}

template <class Item>
bool IteratorBase<Item>::any(::sus::fn::FnMut<bool(Item)> f) noexcept {
  while (true) {
    Option<Item> item = next();
    if (item.is_none()) return false;
    // Safety: `item` was checked to hold Some already.
    if (f(item.take().unwrap_unchecked(::sus::marker::unsafe_fn))) return true;
  }
}

template <class Item>
usize IteratorBase<Item>::count() noexcept {
  auto c = 0_usize;
  while (next().is_some()) c += 1_usize;
  return c;
}

template <class I>
Iterator<Filter<typename I::Item, sizeof(I), alignof(I),
                ::sus::mem::relocate_one_by_memcpy<I>>>
Iterator<I>::filter(
    ::sus::fn::FnMut<bool(const std::remove_reference_t<typename I::Item>&)>
        pred) && noexcept {
  // TODO: make_sized_iterator immediately copies `this` to either the body of
  // the output iterator or to a heap allocation (if it can't be trivially
  // relocated). It is plausible to be more lazy here and avoid moving `this`
  // until it's actually needed, which may not be ever if the resulting iterator
  // is used before `this` gets destroyed. The problem is `this` could be a
  // temporary. So to do this, we could build a doubly-linked list along the
  // chain of iterators. `this` would point to the returned iterator here, and
  // vice versa. If `this` gets destroyed, then we would have to walk the entire
  // linked list and move them all up into the outermost iterator immediately.
  // Doing so dynamically would require a (single) heap allocation at that point
  // always. It would be elided if the iterator was kept on the stack, or used
  // inside the temporary expression. But it would require one heap allocation
  // to use any chain of iterators in a for loop, since temporaies get destroyed
  // after initialing the loop.
  return {::sus::move(pred), make_sized_iterator(static_cast<I&&>(*this))};
}

template <class I>
template <::sus::iter::FromIterator<typename I::Item> C>
C Iterator<I>::collect() && noexcept {
  return C::from_iter(::sus::move(*this));
}

}  // namespace sus::iter
