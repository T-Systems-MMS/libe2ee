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

#ifndef SRC_E2EE_JSON_HPP_
#define SRC_E2EE_JSON_HPP_

#include <rapidjson/document.h>
#include <string>
#include <e2ee/errors.hpp>
#include <e2ee/objects/JsonKey.hpp>

inline std::string __json_get_string(
        const rapidjson::Value& value,
        const e2ee::JsonKey& key,
        const char* file,
        unsigned int line) {
  afgh_check(value.HasMember(key.c_str()), "missing '%s' value", key.c_str());
  afgh_check(value[key.c_str()].IsString(), "invalid '%s' value", key.c_str());
  return value[key.c_str()].GetString();
}
#define JSON_GET_STRING(VALUE, KEY) \
  __json_get_string((VALUE), (KEY), __FILE__, __LINE__)

#endif  // SRC_E2EE_JSON_HPP_
