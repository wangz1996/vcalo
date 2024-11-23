#include "TrackFinder.hh"
#include <random>
#include <stdexcept>

// TrackFinderResult TrackFinder::operator()(
//     const TrackParameters &initialParameters,
//     const TrackFinderOptions &options,
//     TrackContainer &tracks) const
// {
//     return trkFinder.findTracks(initialParameters, options, tracks);
// };

TrackFinderResult TrackFinder::FindTracks(std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
                            //  std::shared_ptr<const Acts::MagneticFieldProvider> magneticField,
                             const TrackParameters &initialParameter,
    const TrackFinderOptions &options,
    TrackContainer &tracks)
{
    auto magneticField = std::make_shared<const Acts::MagneticFieldProvider>(Acts::NullBField());
    Stepper stepper(std::move(magneticField));
    Navigator::Config cfg{std::move(trackingGeometry)};
    cfg.resolvePassive = false;
    cfg.resolveMaterial = true;
    cfg.resolveSensitive = true;
    Navigator navigator(cfg);
    Propagator propagator(std::move(stepper), std::move(navigator));
    CKF trackFinder(std::move(propagator));
    return trackFinder.findTracks(initialParameter, options, tracks);
}

ProcessCode TrackFinder::execute(const Acts::GeometryContext& geoctx,const IndexSourceLinkContainer& sourceLinks, 
std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
    const TrackParametersContainer& initialParameters){
    // Construct a perigee surface as the target surface
    auto pSurface = Acts::Surface::makeShared<Acts::PerigeeSurface>(
        Acts::Vector3{0., 0., 0.});

    Acts::PropagatorPlainOptions pOptions;
    pOptions.maxSteps = 100000;

    Acts::GainMatrixUpdater kfUpdater;
    Acts::GainMatrixSmoother kfSmoother;
    Acts::MeasurementSelector measSel{Acts::MeasurementSelector::Config()};

    Acts::CombinatorialKalmanFilterExtensions<Acts::VectorMultiTrajectory>
        extensions;

    extensions.updater.connect<
        &Acts::GainMatrixUpdater::operator()<Acts::VectorMultiTrajectory>>(
        &kfUpdater);
    extensions.smoother.connect<
        &Acts::GainMatrixSmoother::operator()<Acts::VectorMultiTrajectory>>(
        &kfSmoother);
    extensions.measurementSelector
        .connect<&Acts::MeasurementSelector::select<Acts::VectorMultiTrajectory>>(
            &measSel);

    IndexSourceLinkAccessor slAccessor;
  slAccessor.container = &sourceLinks;
  Acts::SourceLinkAccessorDelegate<IndexSourceLinkAccessor::Iterator>
      slAccessorDelegate;
  slAccessorDelegate.connect<&IndexSourceLinkAccessor::range>(&slAccessor);
    Acts::CalibrationContext calibContext = std::any{};
    std::reference_wrapper<const Acts::CalibrationContext> calibrationContextRef(calibContext);
//Set Finding options
    TrackFinderOptions options(
      geoctx, Acts::MagneticFieldContext(), calibrationContextRef, slAccessorDelegate,
      extensions, pOptions, &(*pSurface));

    // Perform the track finding for all initial parameters

    auto trackContainer = std::make_shared<Acts::VectorTrackContainer>();
  auto trackStateContainer = std::make_shared<Acts::VectorMultiTrajectory>();

  TrackContainer tracks(trackContainer, trackStateContainer);

  tracks.addColumn<unsigned int>("trackGroup");
  Acts::TrackAccessor<unsigned int> seedNumber("trackGroup");

  for (size_t iseed=0; iseed < initialParameters.size(); ++iseed) {
      auto result = 
      this->FindTracks(trackingGeometry,initialParameters.at(iseed), options, tracks);
  }

    return ProcessCode::SUCCESS;

}