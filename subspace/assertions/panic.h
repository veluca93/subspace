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

#include <stdlib.h>

#include "macros/always_inline.h"
#include "macros/builtin.h"
#include "macros/compiler.h"

#if __has_attribute(not_tail_called)
#define _sus__not_tail_called __attribute__((not_tail_called))
#else
#define _sus__not_tail_called
#endif

#if __has_attribute(nomerge)
#define _sus__nomerge __attribute__((nomerge))
#else
#define _sus__nomerge
#endif

#define _sus__crash_attributes _sus__not_tail_called _sus__nomerge

namespace sus {

namespace __private {
void print_panic_message(const char& msg);
}  // namespace __private

/// Terminate the program.
///
/// The default behaviour of this function is to abort(). The behaviour of this
/// function can be overridden by defining a `SUS_PROVIDE_PANIC_HANDLER()` macro
/// when compiling the library.
///
/// # Safety
/// If `SUS_PROVIDE_PANIC_HANDLER()` is defined, the macro _must_ not return of
/// Undefined Behaviour will result.
[[noreturn]] _sus__crash_attributes inline void panic() noexcept {
#if defined(SUS_PROVIDE_PANIC_HANDLER)
  SUS_PROVIDE_PANIC_HANDLER();
#else
  abort();
#endif
}

/// Terminate the program, after printing a message.
///
/// The default behaviour of this function is to print the message to stderr.
/// The behaviour of this function can be overridden by defining a
/// SUS_PROVIDE_PRINT_PANIC_MESSAGE_HANDLER() macro when compiling the library.
///
/// After printing the message, the function will `panic()`.
[[noreturn]] _sus__crash_attributes inline void panic_with_message(
    /* TODO: string view type, or format + args */ const char& msg) noexcept {
#if defined(SUS_PROVIDE_PRINT_PANIC_MESSAGE_HANDLER)
  SUS_PROVIDE_PRINT_PANIC_MESSAGE_HANDLER(msg);
#else
  __private::print_panic_message(msg);
#endif
  panic();
}

}  // namespace sus

#undef _sus__not_tail_called
#undef _sus__nomerge
