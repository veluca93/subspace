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

#include "subspace/fn/fn_box_defn.h"
#include "subspace/iter/iterator_defn.h"
#include "subspace/iter/sized_iterator.h"
#include "subspace/mem/move.h"
#include "subspace/mem/relocate.h"

namespace sus::iter {

using ::sus::mem::relocate_by_memcpy;

/// An iterator to maintain state while iterating another iterator.
///
/// This type is returned from `Iterator::scan()`.
template <class OutType, class State, class InnerSizedIter>
  requires(!std::is_reference_v<State>)
class [[nodiscard]] Scan final
    : public IteratorBase<Scan<OutType, State, InnerSizedIter>, OutType> {
  using Fn = ::sus::fn::FnMutBox<Option<OutType>(
      State&, typename InnerSizedIter::Item&&)>;

 public:
  using Item = OutType;

  // sus::mem::Clone trait.
  Scan clone() const noexcept
    requires(::sus::mem::Clone<State> &&  //
             InnerSizedIter::Clone)
  {
    return Scan(::sus::clone(state_), ::sus::clone(fn_),
                ::sus::clone(next_iter_));
  }

  // sus::iter::Iterator trait.
  Option<Item> next() noexcept {
    Option<Item> out;
    if (Option<typename InnerSizedIter::Item> o = next_iter_.next();
        o.is_some()) {
      out = fn_(state_,
                ::sus::move(o).unwrap_unchecked(::sus::marker::unsafe_fn));
    }
    return out;
  }
  /// sus::iter::Iterator trait.
  ::sus::iter::SizeHint size_hint() const noexcept {
    // Can't know a lower bound, due to the function returning None at any time.
    return ::sus::iter::SizeHint(0u, next_iter_.size_hint().upper);
  }

 private:
  template <class U, class V>
  friend class IteratorBase;

  static Scan with(State&& state, Fn&& fn,
                   InnerSizedIter&& next_iter) noexcept {
    return Scan(::sus::move(state), ::sus::move(fn), ::sus::move(next_iter));
  }

  Scan(State&& state, Fn&& fn, InnerSizedIter&& next_iter) noexcept
      : state_(::sus::move(state)),
        fn_(::sus::move(fn)),
        next_iter_(::sus::move(next_iter)) {}

  State state_;
  Fn fn_;
  InnerSizedIter next_iter_;

  // The InnerSizedIter is trivially relocatable. Likewise, the predicate is
  // known to be trivially relocatable because FnMutBox is. But the State may or
  // may not be.
  sus_class_trivially_relocatable_if_types(::sus::marker::unsafe_fn,
                                           decltype(state_), decltype(fn_),
                                           decltype(next_iter_));
};

}  // namespace sus::iter