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

#ifndef KeyPair_hpp
#define KeyPair_hpp

#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/GlobalParameters.hpp>
#include <memory>

namespace e2ee {
  class KeyPair {
  public:
    KeyPair(std::shared_ptr<Element>& secretKey,
            std::shared_ptr<Element>& publicKey);
    
    KeyPair(std::shared_ptr<GlobalParameters>& global);
    
    std::unique_ptr<Element>
    getReEncryptionKeyFor(const std::shared_ptr<Element>& receiverPublicKey);
    
          std::shared_ptr<Element>& getPublicKey()       { return publicKey; }
    const std::shared_ptr<Element>& getPublicKey() const { return publicKey; }
    
          std::shared_ptr<Element>& getSecretKey()       { return secretKey; }
    const std::shared_ptr<Element>& getSecretKey() const { return secretKey; }
  private:
    std::shared_ptr<Element> secretKey;
    std::shared_ptr<Element> publicKey;
  };
  
}

#endif /* KeyPair_hpp */
