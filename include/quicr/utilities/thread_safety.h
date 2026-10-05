// SPDX-FileCopyrightText: Copyright (c) 2024 Cisco Systems
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

// Probe the standard library implementation. The thread-safety attributes below
// only work when the std synchronization primitives are themselves annotated:
// guarded_by() requires its guard's type to carry the `capability` attribute, and
// std::lock_guard must be `scoped_lockable` for the analysis to see a lock being
// held. Where the std types are unannotated, guarded_by on a plain std::mutex
// fails to compile, and a mutex-wrapping lock()/unlock() pair can't be reconciled
// with the (annotated) inner std::mutex it forwards to.
//
// The host libc++ (macOS/Linux) annotates std::mutex/std::lock_guard whenever the
// compiler supports the capability attributes; libstdc++ never does. We therefore
// require libc++ (_LIBCPP_VERSION) rather than probing a dedicated feature macro —
// there is no portable one (_LIBCPP_HAS_THREAD_SAFETY_ANNOTATIONS does not exist;
// referencing it left this whole block permanently disabled).
//
// The Android NDK is the exception: it ships a libc++ with the annotations off, so
// std::mutex is *not* a capability there even though _LIBCPP_VERSION is defined and
// the NDK clang is >= 21. Enabling our attributes against that unannotated std::mutex
// fails to compile (guarded_by requires a capability-typed guard), so exclude
// Android explicitly. Thread-safety checking still runs on the macOS CI.
#include <version>

#if defined(__clang__) && __clang_major__ >= 21 &&                                                                     \
  (defined(_LIBCPP_VERSION) || _LIBCPP_HAS_THREAD_SAFETY_ANNOTATIONS) && !defined(__ANDROID__)
#define QUICR_CAPABILITY(name) __attribute__((capability(name)))
#define QUICR_ACQUIRE(...) __attribute__((acquire_capability(__VA_ARGS__)))
#define QUICR_RELEASE(...) __attribute__((release_capability(__VA_ARGS__)))
#define QUICR_REQUIRES(...) __attribute__((requires_capability(__VA_ARGS__)))
#define QUICR_TRY_ACQUIRE(...) __attribute__((try_acquire_capability(__VA_ARGS__)))
#define QUICR_GUARDED_BY(...) __attribute__((guarded_by(__VA_ARGS__)))
#define QUICR_PT_GUARDED_BY(...) __attribute__((pt_guarded_by(__VA_ARGS__)))
#define QUICR_NO_THREAD_SAFETY_ANALYSIS __attribute__((no_thread_safety_analysis))
#else
#define QUICR_CAPABILITY(name)
#define QUICR_ACQUIRE(...)
#define QUICR_RELEASE(...)
#define QUICR_REQUIRES(...)
#define QUICR_TRY_ACQUIRE(...)
#define QUICR_GUARDED_BY(...)
#define QUICR_PT_GUARDED_BY(...)
#define QUICR_NO_THREAD_SAFETY_ANALYSIS
#endif
