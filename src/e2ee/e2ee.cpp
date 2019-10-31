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
#include <e2ee/e2ee.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/PbcContext.hpp>
#include <e2ee/objects/KeyPair.hpp>
#include <e2ee/objects/Tuple.hpp>

namespace e2ee {

std::pair<std::string, std::string>
        createKeyPair(int32_t rBits, int32_t qBits) {
  auto context = e2ee::PbcContext::createInstance();
  auto global = std::make_shared<e2ee::GlobalParameters>(context, rBits, qBits);
  auto kp = std::make_unique<e2ee::KeyPair>(global);
  return std::make_pair(
          kp->publicKeyAsJson(),
          kp->secretKeyAsJson()
          );
}

std::string encryptFirstLevel(std::string secretKey) {
  auto context = e2ee::PbcContext::createInstance();
  context->populate(secretKey);
  return "";
}

std::string encryptSecondLevel(std::string secretKey) {
  return "";
}

}