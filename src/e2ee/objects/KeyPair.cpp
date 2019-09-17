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

#include "KeyPair.hpp"
#include <e2ee/objects/GlobalParameters.hpp>

namespace e2ee {
  
  KeyPair::KeyPair(const std::shared_ptr<Element>& secretKey,
                   const std::shared_ptr<Element>& publicKey)
  : secretKey(secretKey), publicKey(publicKey) {
    
  }
  
  KeyPair::KeyPair(const std::shared_ptr<GlobalParameters>& global) {
    auto sk = std::make_shared<Element>(global->lockedContext(),
            global->pairing()->Zr());
    global->lockedContext()->addObject(sk);
    sk->randomize();

    auto pk = *(global->g()) ^ *sk;
    assert(global->g()->field()->getId() == pk->field()->getId());
    secretKey = sk;
    publicKey = pk;
  }

  std::shared_ptr<Element>
  KeyPair::getReEncryptionKeyFor(const std::shared_ptr<Element>& receiverPublicKey) {
    return (*receiverPublicKey) ^ (!(*getSecretKey()));
  }
}
