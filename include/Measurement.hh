#ifndef MEASUREMENT_HH
#define MEASUREMENT_HH

#include <Acts/EventData/Measurement.hpp>
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Definitions/TrackParametrization.hpp"

using Measurement = ::Acts::BoundVariantMeasurement;

using MeasurementContainer = std::vector<Measurement>;

class MeasurementCreator{
public:
    MeasurementCreator() = default;
    ~MeasurementCreator() = default;
    Measurement createMeasurement(
        const Acts::SourceLink& sl, const std::array<Acts::BoundIndices, 2>& indices,
        const Acts::ActsVector<2>& par, const Acts::ActsSymMatrix<2>& cov)
        {return Acts::Measurement<Acts::BoundIndices,2>(sl, indices, par, cov);}
    
private:
};

#endif