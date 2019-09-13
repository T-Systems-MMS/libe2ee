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

namespace e2ee {
class FieldFactory;
}

#ifndef SRC_E2EE_OBJECTS_FIELDFACTORY_HPP_
#define SRC_E2EE_OBJECTS_FIELDFACTORY_HPP_

#include <memory>
#include <utility>
#include <boost/uuid/uuid.hpp>
#include <e2ee/PbcContext.hpp>

namespace e2ee {

class AbstractField;

struct FieldFactory {
  virtual std::shared_ptr<AbstractField> operator()(
          std::shared_ptr<PbcContext> c, const field_s* p) const = 0;

  virtual std::unique_ptr<AbstractField> operator()(
          std::shared_ptr<PbcContext> context,
          const boost::uuids::uuid id,
          const field_s* f,
          bool requireFinal) const = 0;
};

template <class T>
struct FieldFactoryImpl : public FieldFactory {
  std::shared_ptr<AbstractField> operator()(
          std::shared_ptr<PbcContext> c, const field_s* p) const override {
    static_assert(std::is_constructible<T,
                  std::shared_ptr<PbcContext>,
    const boost::uuids::uuid,
    const field_s*,
    bool>::value,
            "type is not constructible with the required arguments");
    return std::make_shared<T>(c, PbcObject::idOf(p), p, true);
  }

  std::unique_ptr<AbstractField> operator() (
          std::shared_ptr<PbcContext> context,
          const boost::uuids::uuid id,
          const field_s* f,
          bool requireFinal) const override {
    static_assert(std::is_constructible<T,
                  std::shared_ptr<PbcContext>,
    const boost::uuids::uuid&,
    const field_s*,
    bool>::value,
            "type is not constructible with the required arguments");
    return std::make_unique<T>(context, id, f, requireFinal);
  }
};
}  // namespace e2ee

#endif  // SRC_E2EE_OBJECTS_FIELDFACTORY_HPP_