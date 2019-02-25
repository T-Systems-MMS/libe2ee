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
  
  KeyPair::KeyPair(std::shared_ptr<Element>& secretKey,
                   std::shared_ptr<Element>& publicKey)
  : secretKey(secretKey), publicKey(publicKey) {
    
  }
  
  KeyPair::KeyPair(std::shared_ptr<GlobalParameters>& global) {
    secretKey = std::make_shared<Element>(global->pairing()->getZr());
    secretKey->randomize();
    publicKey = *(global->g()) ^ *secretKey;
    assert(global->g()->getField()->equals(publicKey->getField()));
  }
  
  std::unique_ptr<Element>
  KeyPair::getReEncryptionKeyFor(const std::shared_ptr<Element>& receiverPublicKey) {
    return *receiverPublicKey ^ ( ! *secretKey);
  }
}
