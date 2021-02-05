/*
 * Copyright 2018-2019 T-Systems Multimedia Solutions GmbH
 *
 * This file is part of libe2ee.
 *
 * libe2ee is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libe2ee is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libe2ee.  If not, see <http://www.gnu.org/licenses/lgpl>.
 */

#ifndef SRC_E2EE_UUID_HPP_
#define SRC_E2EE_UUID_HPP_

#include <string>
#include <boost/uuid/uuid.hpp>

namespace e2ee {
boost::uuids::uuid parse_uuid(const std::string &maybe_uuid,
        bool force_valid = false) {
  static boost::uuids::string_generator gen;

  try {
    auto result = gen(maybe_uuid);
    if (result.version() != boost::uuids::uuid::version_unknown) {
      return result;
    } else if (force_valid) {
      afgh_throw_line("invalid UUID");
    }
  } catch (...) {
    if (force_valid) {
      throw;
    }
    /* ignore and return nil_uuid below */
  }
  return boost::uuids::nil_uuid();
}
}  // namespace e2ee

#endif  // SRC_E2EE_UUID_HPP_
