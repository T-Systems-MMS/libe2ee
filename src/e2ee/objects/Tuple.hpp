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

#ifndef Tuple_hpp
#define Tuple_hpp

#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/GlobalParameters.hpp>
#include <memory>

namespace e2ee {
  class Tuple {
  public:
    Tuple(std::shared_ptr<Element>& message,
          std::shared_ptr<Element>& publicKey,
          std::shared_ptr<GlobalParameters>& global,
          bool secondLevel);
    
    Tuple(std::shared_ptr<Element>& c1,
          std::shared_ptr<Element>& c2,
          const Tuple& base,
          bool secondLevel);
    
    std::unique_ptr<Element> decryptFirstLevel(const std::shared_ptr<Element>& secretKey);
    
    std::unique_ptr<Tuple> reEncrypt(const std::shared_ptr<Element>& rk);
  private:
    bool secondLevel;
    std::shared_ptr<Element> c1;
    std::shared_ptr<Element> c2;
    
    std::shared_ptr<GlobalParameters> global;
  };
}

#endif /* Tuple_hpp */
