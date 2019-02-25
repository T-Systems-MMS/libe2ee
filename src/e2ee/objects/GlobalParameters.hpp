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

#ifndef GlobalParameters_hpp
#define GlobalParameters_hpp

#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/PbcObjectImpl.hpp>
#include <e2ee/ObjectCatalog.hpp>
#include <memory>

namespace e2ee {
  class GlobalParameters {
  public:
    GlobalParameters(int32_t rBits, int32_t qBits);
    
    std::shared_ptr<Pairing> pairing() { return _pairing; }
    std::shared_ptr<Element> g() { return _g; }
    std::shared_ptr<Element> Z() { return _Z; }
    
  private:
    std::shared_ptr<Pairing> _pairing;
    std::shared_ptr<Element> _g;
    std::shared_ptr<Element> _Z;
  };
}

#endif /* GlobalParameters_hpp */
