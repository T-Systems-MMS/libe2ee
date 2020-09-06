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

#include <memory>
#include <vector>
#include <algorithm>
#include <e2ee/e2ee.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/PbcContext.hpp>
#include <e2ee/objects/KeyPair.hpp>
#include <e2ee/objects/Tuple.hpp>
#include <sha2.h>

namespace e2ee {

std::shared_ptr<e2ee::PbcContext> createContext() {
  return e2ee::PbcContext::createInstance();
}


std::shared_ptr<e2ee::GlobalParameters> getGlobal(std::shared_ptr<e2ee::PbcContext> context) {
  return context->global();
}

std::shared_ptr<e2ee::KeyPair>
createKeyPair(std::shared_ptr<e2ee::GlobalParameters> global) {
  return std::make_unique<e2ee::KeyPair>(global);
}
/*
std::pair<std::string, std::string>
        createKeyPair(int32_t rBits, int32_t qBits) {
  auto kp = createKeyPair(createGlobal(rBits, qBits));
  return std::make_pair(
          kp->publicKeyAsJson(),
          kp->secretKeyAsJson()
          );
}
*/

std::shared_ptr<e2ee::Tuple> encryptFirstLevel(std::shared_ptr<e2ee::Element> publicKey, std::shared_ptr<e2ee::Element> dataElement) {
  auto converter = std::dynamic_pointer_cast<e2ee::PbcSerializableField>(publicKey->field());
  auto context = publicKey->getContext().lock();
  return std::make_shared<e2ee::Tuple>(
          dataElement,
          publicKey,
          context->global(),
          false);
}

std::string encryptFirstLevel(std::string publicKey, const std::vector<std::byte>& data) {
  auto context = e2ee::PbcContext::createInstance();
  auto global = std::make_shared<e2ee::GlobalParameters>(context, 160, 512);

  auto key = e2ee::dynamic_pointer_cast<e2ee::Element>(context->populate(publicKey));
  auto converter = std::dynamic_pointer_cast<e2ee::PbcSerializableField>(key->field());
  auto dataElement = converter->elementFromBytes(data.begin(), data.end());
  auto ciphertext = std::make_shared<e2ee::Tuple>(
          dataElement,
          key,
          context->global(),
          false);
  return "";
}


std::shared_ptr<e2ee::Element>
        decryptFirstLevel( std::shared_ptr<e2ee::Element> secretKey, const std::shared_ptr<e2ee::Tuple> ciphertext) {
  return ciphertext->decryptFirstLevel(secretKey);
}

std::vector<std::byte> decryptFirstLevel(std::string secretKey, const std::string& ciphertext) {
  return std::vector<std::byte>();
}

std::shared_ptr<e2ee::Element> generateDataEncryptionKey(std::shared_ptr<e2ee::GlobalParameters> global) {
  auto key = global->pairing()->GT()->randomElement();
  key->randomize();
  return key;
}

std::vector<std::byte> kdf256(std::shared_ptr<e2ee::Element> dek) {
  sha256_ctx ctx;
  sha256_init(&ctx);

  const auto dekBinary = dek->toBytes();
  sha256_update(&ctx,
          reinterpret_cast<const unsigned char*>(&dekBinary[0]),
          dekBinary.size());

  std::vector<std::byte> hash(256/8);
  sha256_final(&ctx, reinterpret_cast<unsigned char*>(&hash[0]));

  return hash;
}

std::shared_ptr<e2ee::Element> publicKey(std::shared_ptr<e2ee::KeyPair> kp) { return kp->getPublicKey(); }
std::shared_ptr<e2ee::Element> secretKey(std::shared_ptr<e2ee::KeyPair> kp) { return kp->getSecretKey(); }

}