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

#include <e2ee/objects/Tuple.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/GlobalParameters.hpp>

namespace e2ee {
  Tuple::Tuple( const std::shared_ptr<Element>& message,
                const std::shared_ptr<Element>& publicKey,
                const std::shared_ptr<GlobalParameters>& global,
                bool secondLevel)
  :   secondLevel(secondLevel), global(global) {
    auto k = std::make_unique<Element>(global->lockedContext(), global->pairing()->Zr());
    k->randomize();
    
    if (secondLevel) {
      _c1 = *publicKey ^ *k;
    } else {
      auto g2 = *(global->g()) ^ *k;
      _c1 = global->pairing()->apply(publicKey, g2);
    }
    auto tmp = (*(global->Z()) ^ *k);
    _c2 = (*message) * (*tmp);
  }
  
  Tuple::Tuple(const std::shared_ptr<Element>& c1,
               const std::shared_ptr<Element>& c2,
               const Tuple& base,
               bool secondLevel)
  :   _c1(c1),
      _c2(c2),
      global(base.global),
      secondLevel(secondLevel) {
  }

  std::shared_ptr<Element> Tuple::decryptFirstLevel(const std::shared_ptr<Element>& secretKey) {
    if (secondLevel) { return nullptr; }
    return *c2() / (*c1() ^(! *secretKey));
  }

  std::shared_ptr<Tuple> Tuple::reEncrypt(const std::shared_ptr<Element>& rk) {
    if (! secondLevel) { return nullptr; }
    auto _c1 = global->pairing()->apply(c1(), rk);
    return std::make_shared<Tuple>(_c1, c2(), *this, false);
  }
}
