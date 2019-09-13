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

#ifndef SRC_E2EE_OBJECTS_PBCOBJECTTYPEIDENTIFIER_HPP_
#define SRC_E2EE_OBJECTS_PBCOBJECTTYPEIDENTIFIER_HPP_

#include <string>
#include <e2ee/objects/PbcObject.hpp>

namespace e2ee {

template<const char* _TYPE, const char* _SUBTYPE>
struct PbcObjectTypeIdentifier : public virtual PbcObject {
  constexpr static std::string_view TYPE = _TYPE;
  constexpr static std::string_view SUBTYPE = _SUBTYPE;

  const std::string& getType() const noexcept {
    return stype;
  }
  const std::string& getSubtype() const noexcept {
    return ssubtype;
  }

 private:
  const std::string stype = _TYPE;
  const std::string ssubtype = _SUBTYPE;
};

}  // namespace e2ee

#endif  // SRC_E2EE_OBJECTS_PBCOBJECTTYPEIDENTIFIER_HPP_
