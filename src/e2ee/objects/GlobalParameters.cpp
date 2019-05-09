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

#include <e2ee/objects/GlobalParameters.hpp>
#include <e2ee/objects/Pairing.hpp>
#include <gmp.h>

namespace e2ee {
  
  GlobalParameters::GlobalParameters(int32_t rBits, int32_t qBits) {

    pbc_set_memory_functions(e2ee_malloc, e2ee_realloc, e2ee_free);
    mp_set_memory_functions(e2ee_malloc, e2ee_realloc2, e2ee_free2);
    _pairing = e2ee::Pairing::generate(160, 512);
    _g = _pairing->initG1();
    _g->randomize();
    _Z = _pairing->apply(_g, _g);
    
    assert(_pairing->isFinal());
    assert(_g->isFinal());
    assert(_Z->isFinal());
  }
}
