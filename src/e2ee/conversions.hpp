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

#ifndef SRC_E2EE_CONVERSIONS_HPP_
#define SRC_E2EE_CONVERSIONS_HPP_
#include <rapidjson/document.h>
#include <gmp.h>
#include <string>
#include <memory>
#include <functional>
#include <array>
#include <boost/uuid/uuid.hpp>
#include <e2ee/errors.hpp>
#include <e2ee/objects/JsonKey.hpp>

namespace e2ee {

extern const std::array<char, 62> FORWARD_CONVERSION_MAP;
extern const size_t CONVERSION_BASE;
extern const std::array<char, 123> REVERSE_CONVERSION_MAP;

char convert_forward(const char &c);

char convert_reverse(const char &c);

std::unique_ptr<mp_limb_t[]> str_to_limbs(
        const std::string::const_iterator& begin,
        const std::string::const_iterator& end, size_t *limbs);

typedef std::unique_ptr<__mpz_struct,
        std::function<void(__mpz_struct *)>> afgh_mpz_t;

afgh_mpz_t str_to_mpz(const std::string::const_iterator& begin,
                      const std::string::const_iterator& end,
                      size_t base = CONVERSION_BASE);

inline
afgh_mpz_t str_to_mpz(const std::string& str, size_t base = CONVERSION_BASE) {
  return str_to_mpz(str.cbegin(), str.cend(), base);
}

std::string mpz_to_str(const mpz_t number);

std::unique_ptr<std::string> limbs_to_str(
        const mp_limb_t *limbs, mp_size_t size);

afgh_mpz_t
json_to_mpz(const rapidjson::Value &jobj);

std::unique_ptr<mp_limb_t[]>
json_to_limbs(const rapidjson::Value &jobj);

boost::uuids::uuid
json_to_uuid(const rapidjson::Value &jobj);

}  // namespace e2ee

#endif  // SRC_E2EE_CONVERSIONS_HPP_
