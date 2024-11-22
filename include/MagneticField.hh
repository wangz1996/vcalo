#ifndef MAGNETICFIELD_HH
#define MAGNETICFIELD_HH

#include "Acts/Definitions/Algebra.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "Acts/MagneticField/NullBField.hpp"
#include "ScalableBField.hh"
#include <memory>
#include <variant>

namespace detail {

using InterpolatedMagneticField2 = Acts::InterpolatedBFieldMap<
    Acts::detail::Grid<Acts::Vector2, Acts::detail::EquidistantAxis,
                       Acts::detail::EquidistantAxis>>;

using InterpolatedMagneticField3 =
    Acts::InterpolatedBFieldMap<Acts::detail::Grid<
        Acts::Vector3, Acts::detail::EquidistantAxis,
        Acts::detail::EquidistantAxis, Acts::detail::EquidistantAxis>>;

}  // namespace detail
#endif