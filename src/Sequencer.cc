#include "Sequencer.hh"
#include "TrackingGeometryView3DBase.hh"

Sequencer::Sequencer(const Config &cfg) : m_cfg(cfg)
{
}

std::shared_ptr<const Acts::TrackingGeometry> Sequencer::buildDetector()
{
  Acts::Logging::Level(Acts::Logging::VERBOSE);
  // Material of the surfaces
  Acts::Material silicon = Acts::Material::fromMassDensity(
      9.370_cm, 46.52_cm, 28.0855, 14, 2.329_g / 1_cm3);

  Acts::MaterialSlab matProp(silicon, 0.35_mm); // Material properties of the surfaces
  const auto surfaceMaterial =
      std::make_shared<Acts::HomogeneousSurfaceMaterial>(matProp); // Surface material
  const auto pBounds =
      std::make_shared<const Acts::RectangleBounds>(142.5_mm, 142.5_mm);
  Acts::RotationMatrix3 rotation = Acts::RotationMatrix3::Identity();
  std::vector<Acts::LayerPtr> layers(nTracker);
  for (size_t i = 0; i < nTracker; ++i)
  {
    Acts::Translation3 trans(0., 0., TrackerPosZ[i]);
    Acts::Transform3 trafo(rotation * trans);
    // Create the detector element
    std::shared_ptr<TelescopeDetectorElement> detElement = std::make_shared<TelescopeDetectorElement>(
        std::make_shared<const Acts::Transform3>(trafo), pBounds, 0.35_mm, surfaceMaterial);
    // Get the surface
    auto surface = detElement->surface().getSharedPtr();
    // std::cout<<"Surface z: "<<surface->center(Acts::GeometryContext()).z()<<std::endl;
    detectorStore.emplace_back(std::move(detElement));

    // Construct the surface array (one surface contained)
    std::unique_ptr<Acts::SurfaceArray> surArray(new Acts::SurfaceArray(surface));
    // Construct the layer
    layers[i] =
        Acts::PlaneLayer::create(trafo, pBounds, std::move(surArray), 0.35_mm);
    // std::cout<<"Layer gid: "<<layers[i]->geometryId().value()<<std::endl;
    auto mutableSurface = const_cast<Acts::Surface *>(surface.get());
    mutableSurface->associateLayer(*layers[i]);
    // std::cout<<"Surface gid: "<<mutableSurface->geometryId().value()<<std::endl;
  }

  // The volume transform
  Acts::Translation3 transVol(0., 0.,// -213.985_mm);
                              (TrackerPosZ[0] + TrackerPosZ[nTracker-1]) * 0.5);
  Acts::Transform3 trafoVol(rotation * transVol);
  auto length = (TrackerPosZ[nTracker-1] - TrackerPosZ[0]);
  // length = 86.28_mm;
  Acts::VolumeBoundsPtr boundsVol = std::make_shared<const Acts::CuboidVolumeBounds>(
      142.5_mm + 5._mm, 142.5_mm + 5._mm, length + 10._mm);
  Acts::LayerArrayCreator::Config lacConfig;
  Acts::LayerArrayCreator layArrCreator(
      lacConfig,
      Acts::getDefaultLogger("LayerArrayCreator", Acts::Logging::INFO));
  Acts::LayerVector layVec;
  for (unsigned int i = 0; i < nTracker; i++)
  {
    layVec.push_back(layers[i]);
  }
//   std::cout<<"Layer number: "<<layVec.size()<<std::endl;

  // Create the layer array
  Acts::GeometryContext genGctx{TelescopeDetectorElement::ContextType()};
  std::unique_ptr<const Acts::LayerArray> layArr(layArrCreator.layerArray(
      genGctx, layVec, TrackerPosZ[0] - 2._mm, TrackerPosZ[nTracker-1] + 2._mm,
      Acts::BinningType::equidistant, Acts::BinningValue::binZ));
  // std::unique_ptr<const Acts::LayerArray> layArr(layArrCreator.layerArray(
  //     genGctx, layVec, -259.125_mm, -168.845_mm,
  //     Acts::BinningType::equidistant, Acts::BinningValue::binZ));
      //TODO
    m_geoctx = genGctx;
  auto layContainer = layArr->arrayObjects();
  // 
  // Build the tracking volume
  auto trackVolume =
      Acts::TrackingVolume::create(trafoVol, boundsVol, nullptr,
                                   std::move(layArr), nullptr, {}, "Telescope");
  // Build and return tracking geometry
  auto printgid = [](const auto& gid){
    std::cout<<"Volume gid: "<<gid.volume()<<" "<<gid.boundary()<<" "<<gid.layer()<<" "<<gid.approach()<<" "<<gid.sensitive()<<" "<<gid.extra()<<std::endl;
  };
  auto trackGeometry = std::make_shared<Acts::TrackingGeometry>(trackVolume);
  for(int i=0;i<nTracker;i++){
    Acts::GeometryIdentifier genGid;
    genGid.setVolume(1);genGid.setLayer(i+1);genGid.setSensitive(1);
    auto corsurface = trackGeometry->findSurface(genGid);
    m_surfacegids.emplace_back(std::move(genGid));
  }

  //Visualization
  Acts::ObjVisualization3D obj;
  // std::cout<<"Start to run 3D viewer"<<std::endl;
  // std::cout<<Acts::TrackingGeometryView3DTest::run(*(trackVolume.get()),obj,true,"")<<std::endl;
  return std::make_shared<Acts::TrackingGeometry>(trackVolume);
}

int Sequencer::run()
{
    return 1;
}

void Sequencer::addElement(std::shared_ptr<SequenceElement> &element)
{
  m_sequenceElements.emplace_back(std::move(element));
}