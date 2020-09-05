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


#ifndef LIBE2EE_E2EE_HPP
#define LIBE2EE_E2EE_HPP

#include <string>

namespace e2ee {

class GlobalParameters;
class Element;
class Tuple;
class KeyPair;

std::shared_ptr<e2ee::GlobalParameters> createGlobal(int32_t rBits, int32_t qBits);

std::unique_ptr<e2ee::KeyPair> createKeyPair(std::shared_ptr<e2ee::GlobalParameters> global);
std::pair<std::string, std::string> createKeyPair(int32_t rBits, int32_t qBits);

std::shared_ptr<e2ee::Tuple> encryptFirstLevel(std::shared_ptr<e2ee::Element> publicKey, std::shared_ptr<e2ee::Element> dataElement);
std::string encryptFirstLevel(std::string publicKey, const std::vector<std::byte>& data);
std::vector<std::byte> decryptFirstLevel(std::string secretKey, const std::string& ciphertext);

std::shared_ptr<e2ee::Element> generateDataEncryptionKey(std::shared_ptr<e2ee::GlobalParameters> global);
std::vector<std::byte> kdf(std::shared_ptr<e2ee::Element> dek, std::size_t length);

}

#endif //LIBE2EE_E2EE_HPP
