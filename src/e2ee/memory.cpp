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

#include <e2ee/memory.hpp>
#include <cstdlib>
#include <map>

static
std::map<void*, size_t> used_addresses;

extern "C" {
int e2ee_isvalid(void* ptr) {
  auto iter = used_addresses.find(ptr);
  if (iter == used_addresses.end()) {
    return 0;
  } else {
    return 1;
  }
}

/* release memory got from pbc_malloc only by e2ee_free(), do not use free() */
void * e2ee_malloc(size_t size) {
  void* ptr = std::malloc(size);
  std::memset(ptr, 0, size);
  used_addresses[ptr] = size;
  return ptr;
}

/* e2ee_realloc guarantees zeroing out the memory before moving old memory */

void * e2ee_realloc2(void * ptr, size_t /* oldsize */, size_t newsize) {
  return e2ee_realloc(ptr, newsize);
}
void * e2ee_realloc(void * ptr, size_t size) {
  if (ptr == NULL) {
    return e2ee_malloc(size);
  }

  auto iter = used_addresses.find(ptr);
  if (iter == used_addresses.end()) {
    return NULL;
  } else {
    void* newptr = std::realloc(ptr, size);

    if (newptr != ptr) {
      used_addresses.erase(iter);
      used_addresses[newptr] = true;
    }

    return newptr;
  }
  return NULL;
}

/* e2ee_free guarantees zeroing out the memory */
void e2ee_free2(void * ptr, size_t /* size */) {
  return e2ee_free(ptr);
}
/* e2ee_free guarantees zeroing out the memory */
void e2ee_free(void * ptr) {
  auto iter = used_addresses.find(ptr);
  if (iter != used_addresses.end()) {
    std::memset(iter->first, 0, iter->second);
    free(ptr);
    used_addresses.erase(iter);
  }
}

}