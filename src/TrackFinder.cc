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
                             const Acts::CurvilinearTrackParameters &initialParameter,
    const TrackFinderOptions &options,
    TrackContainer &tracks)
{
    // auto magneticField = std::make_shared<const Acts::NullBField>(Acts::NullBField());
    // Acts::Vector3 magneticFieldVector(0, 0, 1 * Acts::UnitConstants::T);
    // auto magneticField = std::make_shared<const Acts::ConstantBField>(magneticFieldVector);
    // Stepper stepper(std::move(magneticField));
    auto field =
        std::make_shared<Acts::ConstantBField>(Acts::Vector3(0.0, 0.0, 0.0 * Acts::UnitConstants::T));
    ConstantFieldStepper stepper(std::move(field));
    Navigator::Config cfg{std::move(trackingGeometry)};
    cfg.resolvePassive = false;
    cfg.resolveMaterial = true;
    cfg.resolveSensitive = true;
    Navigator navigator(cfg);
    ConstantFieldPropagator propagator(std::move(stepper), std::move(navigator));
    CKF trackFinder(std::move(propagator), Acts::getDefaultLogger("CKF",Acts::Logging::VERBOSE));
    return trackFinder.findTracks(initialParameter, options, tracks);
}

ProcessCode TrackFinder::execute(const Acts::GeometryContext& geoctx,const IndexSourceLinkContainer& sourceLinks, 
std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
    const std::vector<Acts::CurvilinearTrackParameters>& initialParameters, const MeasurementContainer& measurements){
    // Construct a perigee surface as the target surface
    // auto pSurface = Acts::Surface::makeShared<Acts::PerigeeSurface>(
    //     Acts::Vector3{0., 0., 0.});
    auto pSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(
      Acts::Vector3{0., 0., -100._mm}, Acts::Vector3{0., 0., 1.});
    Acts::PropagatorPlainOptions pOptions;
    // pOptions.pathLimit = 1000._mm;
    // pOptions.maxStepSize = 5._mm;
    pOptions.maxSteps = 1000;
    PassThroughCalibrator pcalibrator;
    MeasurementCalibratorAdapter calibrator(pcalibrator, measurements);
    Acts::GainMatrixUpdater kfUpdater;
    Acts::GainMatrixSmoother kfSmoother;
    Acts::MeasurementSelector measSel{Acts::MeasurementSelector::Config()};

    Acts::CombinatorialKalmanFilterExtensions<Acts::VectorMultiTrajectory>
        extensions;
    extensions.calibrator.connect<&MeasurementCalibratorAdapter::calibrate>(
      &calibrator);
    extensions.updater.connect<
        &Acts::GainMatrixUpdater::operator()<Acts::VectorMultiTrajectory>>(
        &kfUpdater);
    extensions.smoother.connect<
        &Acts::GainMatrixSmoother::operator()<Acts::VectorMultiTrajectory>>(
        &kfSmoother);
    // extensions.measurementSelector
    //     .connect<&Acts::MeasurementSelector::select<Acts::VectorMultiTrajectory>>(
    //         &measSel);

    IndexSourceLinkAccessor slAccessor;
  slAccessor.container = &sourceLinks;
  Acts::SourceLinkAccessorDelegate<IndexSourceLinkAccessor::Iterator>
      slAccessorDelegate;
  slAccessorDelegate.connect<&IndexSourceLinkAccessor::range>(&slAccessor);
  WhiteBoard WB;
  AlgorithmContext algoctx(1, 1, WB);
    // std::reference_wrapper<const Acts::CalibrationContext> calibrationContext;
//Set Finding options
    TrackFinderOptions options(
      geoctx, Acts::MagneticFieldContext(), algoctx.calibContext, slAccessorDelegate,
      extensions, pOptions, &(*pSurface));

    options.propagatorPlainOptions.direction = Acts::Direction::Forward;

    // Perform the track finding for all initial parameters

    auto trackContainer = std::make_shared<Acts::VectorTrackContainer>();
  auto trackStateContainer = std::make_shared<Acts::VectorMultiTrajectory>();

  TrackContainer tracks(trackContainer, trackStateContainer);

  tracks.addColumn<unsigned int>("trackGroup");
  Acts::TrackAccessor<unsigned int> seedNumber("trackGroup");

  for (size_t iseed=0; iseed < initialParameters.size(); ++iseed) {
      auto result = 
      this->FindTracks(trackingGeometry,initialParameters.at(iseed), options, tracks);
      if(not result.ok()){
        std::cout<<"Following error info"<<std::endl;
        std::cout<<result.error()<<" "<<result.error().message()<<std::endl;
      }
  }
  auto constTrackStateContainer =
      std::make_shared<Acts::ConstVectorMultiTrajectory>(
          std::move(*trackStateContainer));

  auto constTrackContainer = std::make_shared<Acts::ConstVectorTrackContainer>(
      std::move(*trackContainer));

  ConstTrackContainer constTracks{constTrackContainer,
                                  constTrackStateContainer};
    
    std::cout<<"N Tracks found: "<<constTracks.size()<<std::endl;
    std::cout<<"Starting to resolve the ambiguity"<<std::endl;
    for(size_t i=0;i<constTracks.size();i++){
        auto track = constTracks.getTrack(i);
        std::cout<<"has ref surface: "<<track.hasReferenceSurface()<<std::endl;
        auto *sur = &track.referenceSurface();
        auto planesur = dynamic_cast<const Acts::PlaneSurface*>(sur);
        auto poscen = planesur->center(geoctx);
        std::cout<<"ref surface center: "<<poscen.x()<<" "<<poscen.y()<<" "<<poscen.z()<<std::endl;
        auto unitDir = track.unitDirection();
        std::cout<<"unit direction"<<unitDir.x()<<" "<<unitDir.y()<<" "<<unitDir.z()<<std::endl;
        auto nmeas = track.nMeasurements();
        std::cout<<"n meas: "<<nmeas<<std::endl;
    }

    AmbiguitySolver->execute(constTracks);
    
    return ProcessCode::SUCCESS;

}