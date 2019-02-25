/*
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

#ifndef JsonKey_hpp
#define JsonKey_hpp

#include <string>

namespace e2ee {
  class JsonKey {
  public:
    JsonKey() = delete;
    explicit JsonKey(const std::string& name) : value(name) {}
    const std::string& str() const noexcept { return value; }
    const char* c_str() const noexcept { return value.c_str(); }
    
    bool operator==(const JsonKey& key) const noexcept { return value == key.value; }
    bool operator!=(const JsonKey& key) const noexcept { return value != key.value; }
    bool operator==(const std::string& key) const noexcept { return value == key; }
    bool operator!=(const std::string& key) const noexcept { return value != key; }
  private:
    std::string value;
  };

  const static JsonKey KEY_ID("id");
  const static JsonKey KEY_TYPE("type");
  const static JsonKey KEY_SUBTYPE("subtype");
  const static JsonKey KEY_ORDER("order");
  const static JsonKey KEY_ROOT("root");
  const static JsonKey KEY_FIELD("field");
  const static JsonKey KEY_BASE("base");
  const static JsonKey KEY_MODULUS("modulus");
  const static JsonKey KEY_A("a");
  const static JsonKey KEY_B("b");
  const static JsonKey KEY_x("x");
  const static JsonKey KEY_y("y");
  const static JsonKey KEY_GEN("gen");
  const static JsonKey KEY_GENNOCOFAC("gen_no_cofac");
  const static JsonKey KEY_COFAC("cofac");
  const static JsonKey KEY_QUOTIENTCMP("quotient_cmp");
  const static JsonKey KEY_NEGPINV("negpinv");
  const static JsonKey KEY_R("R");
  const static JsonKey KEY_R3("R3");
  const static JsonKey KEY_G1("G1");
  const static JsonKey KEY_G2("G2");
  const static JsonKey KEY_GT("GT");
  const static JsonKey KEY_Zr("Zr");
  const static JsonKey KEY_r("r");
  const static JsonKey KEY_phikonr("phikonr");
  const static JsonKey KEY_PAIRING("pairing");

}
#endif /* JsonKey_hpp */
