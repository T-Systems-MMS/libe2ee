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

GlobalParameters::GlobalParameters(std::weak_ptr<PbcContext> ctx, int32_t rBits, int32_t qBits):
  context(ctx) {

  pbc_set_memory_functions(e2ee_malloc, e2ee_realloc, e2ee_free);
  mp_set_memory_functions(e2ee_malloc, e2ee_realloc2, e2ee_free2);
  if (auto c = ctx.lock()) {
    auto p = c->createPairing(160, 512);
    _pairing = p;

    auto g = p->initG1();
    g->randomize();
    _g = g;

    auto Z = p->apply(g, g);
    _Z = Z;

    assert(p->isFinal());
    assert(g->isFinal());
    assert(Z->isFinal());
    assert(p->G1()->isFinal());
    assert(p->G2()->isFinal());
    assert(p->GT()->isFinal());
    assert(p->Zr()->isFinal());
  } else {
    afgh_throw_line("unable to acquire context lock");
  }
}
}  // namespace e2ee
