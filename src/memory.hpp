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

#ifndef afgh_memory_h
#define afgh_memory_h

#include <e2ee/errors.hpp>
#include <cstdlib>
#include <memory>
/*
template <typename T>
std::shared_ptr<T>
allocate_managed() {
  T* data = (T*) std::malloc(sizeof(T));
  if (data == NULL) {
    throw_line("unable to allocate memory");
  } else {
    bzero(data, sizeof(T));
  }
  return std::shared_ptr<T>(data, std::free);
}
*/

template <typename T>
T*
allocate_unmanaged(size_t count=1) {
  T* data = (T*) std::malloc(sizeof(T)*count);
  if (data == NULL) {
    afgh_throw_line("unable to allocate memory");
  } else {
    bzero(data, sizeof(T)*count);
  }
  return data;
}

#endif /* afgh_memory_h */
