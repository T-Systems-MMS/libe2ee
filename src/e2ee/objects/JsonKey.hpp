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

#ifndef SRC_E2EE_OBJECTS_JSONKEY_HPP_
#define SRC_E2EE_OBJECTS_JSONKEY_HPP_

#include <string_view>
#include <memory>

namespace e2ee {
class JsonKey {
 public:
  JsonKey() = delete;

  explicit constexpr JsonKey(const std::string_view& name) noexcept
  : string_value(name) {}

  const std::string_view &str() const noexcept { return string_value; }
  const char *c_str() const noexcept { return string_value.data(); }

  operator rapidjson::Value () const {
    rapidjson::Value json_value;
    json_value.SetString(string_value.data(),
            string_value.length(), allocator());
    return json_value;
  }

  operator const std::string_view &() const noexcept { return string_value; }
  operator const char *() const noexcept { return string_value.data(); }

  bool operator==(const JsonKey &key) const noexcept {
    return string_value == key.string_value;
  }

  bool operator!=(const JsonKey &key) const noexcept {
    return string_value != key.string_value;
  }

  bool operator==(const std::string_view &key) const noexcept {
    return string_value == key;
  }

  bool operator!=(const std::string_view &key) const noexcept {
    return string_value != key;
  }

 private:
  static rapidjson::MemoryPoolAllocator<>& allocator() {
    static rapidjson::Document doc;
    if (!doc.IsObject()) {
      doc.SetObject();
    }
    return doc.GetAllocator();
  }

  std::string_view string_value;
};

static constexpr JsonKey KEY_ID("id");                      // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_TYPE("type");                  // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_SUBTYPE("subtype");            // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_ORDER("order");                // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_ROOT("root");                  // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_FIELD("field");                // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_BASE("base");                  // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_MODULUS("modulus");            // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_A("a");                        // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_B("b");                        // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_x("x");                        // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_y("y");                        // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_GEN("gen");                    // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_GENNOCOFAC("gen_no_cofac");    // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_COFAC("cofac");                // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_QUOTIENTCMP("quotient_cmp");   // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_NEGPINV("negpinv");            // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_R("R");                        // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_R3("R3");                      // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_G1("G1");                      // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_G2("G2");                      // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_GT("GT");                      // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_Zr("Zr");                      // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_r("r");                        // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_phikonr("phikonr");            // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_PAIRING("pairing");            // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_Eq("Eq");                      // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_Fq("Fq");                      // NOLINT: cert-err58-cpp
static constexpr JsonKey KEY_Fq2("Fq2");                    // NOLINT: cert-err58-cpp

}  // namespace e2ee
#endif  // SRC_E2EE_OBJECTS_JSONKEY_HPP_
