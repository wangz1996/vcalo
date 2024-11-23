#ifndef TRACKFINDER_HH
#define TRACKFINDER_HH
#include "TrackFindingAlgorithm.hh"
#include "Acts/EventData/TrackContainer.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"
#include "Acts/EventData/VectorTrackContainer.hpp"
#include "Acts/MagneticField/MagneticFieldProvider.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/TrackFitting/GainMatrixSmoother.hpp"
#include "Acts/TrackFitting/GainMatrixUpdater.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/MagneticField/NullBField.hpp"
#include "ProcessCode.hh"

using Updater = Acts::GainMatrixUpdater;
using Smoother = Acts::GainMatrixSmoother;

using Stepper = Acts::EigenStepper<>;
using Navigator = Acts::Navigator;
using Propagator = Acts::Propagator<Stepper, Navigator>;
using CKF =
    Acts::CombinatorialKalmanFilter<Propagator, Acts::VectorMultiTrajectory>;

using TrackContainer =
    Acts::TrackContainer<Acts::VectorTrackContainer,
                         Acts::VectorMultiTrajectory, std::shared_ptr>;
using TrackFinderOptions =
    Acts::CombinatorialKalmanFilterOptions<IndexSourceLinkAccessor::Iterator,
                                           Acts::VectorMultiTrajectory>;
using TrackFinderResult =
    Acts::Result<std::vector<TrackContainer::TrackProxy>>;
class TrackFinder
{
public:
    TrackFinder() = default;
    virtual ~TrackFinder() = default;
    virtual void Print(){std::cout<<"Hello World"<<std::endl;}
    // TrackFinderResult operator()(const TrackParameters &,
    //                                      const TrackFinderOptions &,
    //                                      TrackContainer &) const ;
    TrackFinderResult FindTracks(std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
                            // std::shared_ptr<const Acts::MagneticFieldProvider> magneticField,
                            const TrackParameters &initialParameters,
    const TrackFinderOptions &options,
    TrackContainer &tracks);

    ProcessCode execute(const Acts::GeometryContext& geoctx,const IndexSourceLinkContainer& sourceLinks,
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
    const TrackParametersContainer& initialParameters);

private:
    // CKF trkFinder(Propagator());
};

#endif