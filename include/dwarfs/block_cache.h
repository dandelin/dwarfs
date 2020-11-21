/* vim:set ts=2 sw=2 sts=2 et: */
/**
 * \author     Marcus Holland-Moritz (github@mhxnet.de)
 * \copyright  Copyright (c) Marcus Holland-Moritz
 *
 * This file is part of dwarfs.
 *
 * dwarfs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * dwarfs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dwarfs.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <future>
#include <limits>
#include <memory>
#include <mutex>

#include "fstypes.h"
#include "logger.h"

namespace dwarfs {

struct block_cache_options;

class cached_block;

class block_range {
 public:
  block_range(std::shared_ptr<cached_block const> block, size_t offset,
              size_t size);

  const uint8_t* data() const { return begin_; }
  const uint8_t* begin() const { return begin_; }
  const uint8_t* end() const { return end_; }
  size_t size() const { return end_ - begin_; }

 private:
  const uint8_t* const begin_;
  const uint8_t* const end_;
  std::shared_ptr<cached_block const> block_;
};

class block_cache {
 public:
  block_cache(logger& lgr, const block_cache_options& options);

  size_t block_count() const { return impl_->block_count(); }

  void insert(compression_type comp, const uint8_t* data, size_t size) {
    impl_->insert(comp, data, size);
  }

  void set_block_size(size_t size) { impl_->set_block_size(size); }

  std::future<block_range>
  get(size_t block_no, size_t offset, size_t size) const {
    return impl_->get(block_no, offset, size);
  }

  class impl {
   public:
    virtual ~impl() = default;

    virtual size_t block_count() const = 0;
    virtual void
    insert(compression_type comp, const uint8_t* data, size_t size) = 0;
    virtual void set_block_size(size_t size) = 0;
    virtual std::future<block_range>
    get(size_t block_no, size_t offset, size_t length) const = 0;
  };

 private:
  std::unique_ptr<impl> impl_;
};
} // namespace dwarfs