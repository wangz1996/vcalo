#include "Sequencer.hh"

Sequencer::Sequencer(const Config &cfg) : m_cfg(cfg)
{
}

std::shared_ptr<const Acts::TrackingGeometry> Sequencer::buildDetector()
{
  // Material of the surfaces
  Acts::Material silicon = Acts::Material::fromMassDensity(
      9.370_cm, 46.52_cm, 28.0855, 14, 2.329_g / 1_cm3);

  Acts::MaterialSlab matProp(silicon, 0.35_mm); // Material properties of the surfaces
  const auto surfaceMaterial =
      std::make_shared<Acts::HomogeneousSurfaceMaterial>(matProp); // Surface material
  const auto pBounds =
      std::make_shared<const Acts::RectangleBounds>(142.5_mm, 142.5_mm);
  Acts::RotationMatrix3 rotation = Acts::RotationMatrix3::Identity();
  std::vector<Acts::LayerPtr> layers(6);
  for (size_t i = 0; i < 6; ++i)
  {
    Acts::Translation3 trans(0., 0., TrackerPosZ[i]);
    Acts::Transform3 trafo(rotation * trans);
    // Create the detector element
    std::shared_ptr<TelescopeDetectorElement> detElement = std::make_shared<TelescopeDetectorElement>(
        std::make_shared<const Acts::Transform3>(trafo), pBounds, 0.35_mm, surfaceMaterial);
    // Get the surface
    auto surface = detElement->surface().getSharedPtr();
    detectorStore.emplace_back(std::move(detElement));

    // Construct the surface array (one surface contained)
    std::unique_ptr<Acts::SurfaceArray> surArray(new Acts::SurfaceArray(surface));
    // Construct the layer
    layers[i] =
        Acts::PlaneLayer::create(trafo, pBounds, std::move(surArray), 0.35_mm);
    auto mutableSurface = const_cast<Acts::Surface *>(surface.get());
    mutableSurface->associateLayer(*layers[i]);
  }

  // Define volume
  //   Detector
  // └── Volume (例如：一个子探测器模块)
  //       ├── BoundarySurfaces (Volume 的边界表面)
  //       │     ├── PlaneSurface
  //       │     ├── CylinderSurface
  //       │     └── DiscSurface
  //       ├── Layers (Volume 内的层)
  //       │     └── Surfaces (每层包含多个 Surface)
  //       │           ├── Detector Surface (感应表面)
  //       │           └── Passive Surface (非交互表面)
  //       └── Material Properties (材料属性)

  // The volume transform
  Acts::Translation3 transVol(0., 0.,
                              (TrackerPosZ[0] + TrackerPosZ[5]) * 0.5);
  Acts::Transform3 trafoVol(rotation * transVol);
  auto length = TrackerPosZ[5] - TrackerPosZ[0];
  Acts::VolumeBoundsPtr boundsVol = std::make_shared<const Acts::CuboidVolumeBounds>(
      142.5_mm + 5._mm, 142.5_mm + 5._mm, length + 10._mm);
  Acts::LayerArrayCreator::Config lacConfig;
  Acts::LayerArrayCreator layArrCreator(
      lacConfig,
      Acts::getDefaultLogger("LayerArrayCreator", Acts::Logging::INFO));
  Acts::LayerVector layVec;
  for (unsigned int i = 0; i < 6; i++)
  {
    layVec.push_back(layers[i]);
  }
//   std::cout<<"Layer number: "<<layVec.size()<<std::endl;

  // Create the layer array
  Acts::GeometryContext genGctx{TelescopeDetectorElement::ContextType()};
  std::unique_ptr<const Acts::LayerArray> layArr(layArrCreator.layerArray(
      genGctx, layVec, TrackerPosZ[0] - 2._mm, TrackerPosZ[5] + 2._mm,
      Acts::BinningType::equidistant, Acts::BinningValue::binZ));
    m_geoctx = genGctx;
  auto layContainer = layArr->arrayObjects();
//   for(auto layer:layContainer){
//     std::cout<<"Layer ptr: "<<layer-> <<std::endl;
//   }
  // Build the tracking volume
  auto trackVolume =
      Acts::TrackingVolume::create(trafoVol, boundsVol, nullptr,
                                   std::move(layArr), nullptr, {}, "Telescope");
  // Build and return tracking geometry
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