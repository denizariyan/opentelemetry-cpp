// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>  // For std::size_t

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include "google/protobuf/arena.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace detail
{

/**
 * Alignment protobuf requires of an arena's initial block.
 */
constexpr std::size_t kArenaBlockAlignment = 8;

/**
 * Storage for an arena's first block, carrying both its size and the alignment protobuf requires.
 *
 * Size it per message type from google::protobuf::Arena::SpaceUsed(), leaving roughly 25% headroom
 * to reduce unnecessary allocations.
 */
template <std::size_t Bytes>
struct alignas(kArenaBlockAlignment) InlineArenaBlock
{
  char bytes[Bytes];
};

/**
 * Options for an arena whose first block is storage inside the owning object.
 *
 * @param initial_block Storage for the arena's first block. It must outlive the arena.
 * @param max_block_size Cap on the blocks the arena allocates once it outgrows the initial block.
 *        Protobuf otherwise grows geometrically from the initial block's size, so a small spill
 *        leads to an unnecessarily large allocation. Trades more allocations on very large
 *        messages for less wasted memory on typical ones.
 */
template <std::size_t Bytes>
google::protobuf::ArenaOptions MakeArenaOptions(InlineArenaBlock<Bytes> &initial_block,
                                                std::size_t max_block_size) noexcept
{
  google::protobuf::ArenaOptions options;
  options.initial_block      = initial_block.bytes;
  options.initial_block_size = Bytes;
  options.max_block_size     = max_block_size;
  return options;
}

}  // namespace detail
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
