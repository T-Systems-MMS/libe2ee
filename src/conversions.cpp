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

#include <e2ee/conversions.hpp>
#include <e2ee/errors.hpp>
#include <e2ee/memory.hpp>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <gmp.h>

namespace e2ee {
  
  const std::array<char, 62> FORWARD_CONVERSION_MAP = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z'
  };
  
  const size_t CONVERSION_BASE = FORWARD_CONVERSION_MAP.size();
  
  const std::array<char, 123> REVERSE_CONVERSION_MAP = {
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, /*   0 -   9 */
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, /*  10 -  19 */
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, /*  20 -  29 */
    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, /*  30 -  39 */
    99, 99, 99, 99, 99, 99, 99, 99,  0,  1, /*  40 -  49 */
     2,  3,  4,  5,  6,  7,  8,  9, 99, 99, /*  50 -  59 */
    99, 99, 99, 99, 99, 10, 11, 12, 13, 14, /*  60 -  69 */
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, /*  70 -  79 */
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, /*  80 -  89 */
    35, 99, 99, 99, 99, 99, 99, 36, 37, 38, /*  90 -  99 */
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, /* 100 - 109 */
    49, 50, 51, 52, 53, 54, 55, 56, 57, 58, /* 110 - 119 */
    59, 60, 61                              /* 120 - 122 */
  };
  
  char convert_forward(const char& c) {
    if (c > FORWARD_CONVERSION_MAP.size()) {
      afgh_throw_line("unable to convert number: '0x%02x'", c);
    }
    return FORWARD_CONVERSION_MAP[c];
  }
  
  char convert_reverse(const char& c) {
    if (c > REVERSE_CONVERSION_MAP.size()) {
      afgh_throw_line("unable to convert character: '0x%02x'", c);
    }
    return REVERSE_CONVERSION_MAP[c];
  }
  
  std::unique_ptr<std::string> limbs_to_str(const mp_limb_t* limbs, mp_size_t size) {
    const size_t buffer_size = 1 + mpn_sizeinbase(limbs, size, CONVERSION_BASE);
    char buffer[buffer_size];
    bzero(&buffer[0], buffer_size);
    mp_size_t characters = mpn_get_str((unsigned char*)&buffer[0], CONVERSION_BASE, (mp_limb_t*)limbs, size);
    assert(characters+1 <= buffer_size);
    std::transform(&buffer[0], &buffer[characters], &buffer[0], convert_forward);
    assert(buffer[characters] == '\0');
    return std::make_unique<std::string>(&buffer[0]);
  }
  
  mp_limb_t *str_to_limbs(const std::string::const_iterator begin,
                          const std::string::const_iterator end,
                          size_t* limbs) {
    std::vector<unsigned char> strTemp(std::distance(begin, end));
    std::transform(begin, end, strTemp.begin(), convert_reverse);
    mp_limb_t* m = allocate_unmanaged<mp_limb_t>(strTemp.size()+1);
    *limbs = mpn_set_str(m, &strTemp[0], strTemp.size(), CONVERSION_BASE);
    return m;
  }
  
  afgh_mpz_t
  str_to_mpz(const std::string::const_iterator begin,
             const std::string::const_iterator end,
             size_t base) {
    afgh_mpz_t m(new __mpz_struct[1], [](__mpz_struct *ptr){mpz_clear(ptr);});
    mpz_init(m.get());
    if (begin >= end) {
      afgh_throw_line("Json object is not a string");
    }
    
    if (std::distance(begin, end) == 1 && *begin == 'O') {
      mpz_set_str(m.get(), "0", static_cast<int>(base));
    } else if (0 != mpz_set_str(m.get(), &(*begin), static_cast<int>(base))) {
      afgh_throw_line("unable to convert '%s' to mpz_t", &(*begin));
    }
    return m;
  }
  
  std::unique_ptr<std::string> mpz_to_str(const mpz_t number) {
    mp_size_t bytes = mpz_sizeinbase(number, CONVERSION_BASE) + 2;
    char buffer[bytes];
    bzero(buffer, bytes);
    mpz_get_str(&buffer[0], CONVERSION_BASE, number);
    return std::make_unique<std::string>(&buffer[0]);
  }
}
