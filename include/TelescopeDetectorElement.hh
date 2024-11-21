#ifndef TELESCOPEDETECTORELEMENT_HH
#define TELESCOPEDETECTORELEMENT_HH
#include "Acts/Geometry/DetectorElementBase.hpp"

namespace Acts {
class Surface;
class PlanarBounds;
class DiscBounds;
class ISurfaceMaterial;
}  // namespace Acts
class TelescopeDetectorElement : public Acts::DetectorElementBase{
public:
    /// @class ContextType
  /// convention: nested to the Detector element
  struct ContextType {
    /// The current intervall of validity
    unsigned int iov = 0;
  };
    TelescopeDetectorElement(
        std::shared_ptr<const Acts::Transform3> transform,
        std::shared_ptr<const Acts::PlanarBounds> pBounds, double thickness,
        std::shared_ptr<const Acts::ISurfaceMaterial> material = nullptr);
    ~TelescopeDetectorElement() override = default;
    Acts::Surface& surface() final;
    const Acts::Surface& surface() const final;
    /// The maximal thickness of the detector element wrt normal axis
  double thickness() const final;

  /// Return local to global transform associated with this identifier
  ///
  /// @param gctx The current geometry context object, e.g. alignment
  ///
  /// @note this is called from the surface().transform() in the PROXY mode
  const Acts::Transform3& transform(
      const Acts::GeometryContext& gctx) const final;

  /// Return the nominal local to global transform
  ///
  /// @note the geometry context will hereby be ignored
  const Acts::Transform3& nominalTransform(
      const Acts::GeometryContext& gctx) const;

  /// Return local to global transform associated with this identifier
  ///
  /// @param alignedTransform is a new transform
  /// @oaram iov is the batch for which it is meant
  void addAlignedTransform(std::unique_ptr<Acts::Transform3> alignedTransform,
                           unsigned int iov);

  /// Return the set of alignment transforms in flight
  const std::vector<std::unique_ptr<Acts::Transform3>>& alignedTransforms()
      const;

private:
  /// the transform for positioning in 3D space
  std::shared_ptr<const Acts::Transform3> m_elementTransform = nullptr;
  // the aligned transforms
  std::vector<std::unique_ptr<Acts::Transform3>> m_alignedTransforms = {};
  /// the surface represented by it
  std::shared_ptr<Acts::Surface> m_elementSurface = nullptr;
  /// the element thickness
  double m_elementThickness = 0.;
  /// the planar bounds
  std::shared_ptr<const Acts::PlanarBounds> m_elementPlanarBounds = nullptr;
  /// the disc bounds
  std::shared_ptr<const Acts::DiscBounds> m_elementDiscBounds = nullptr;
};
inline const Acts::Surface& TelescopeDetectorElement::surface() const {
  return *m_elementSurface;
}

inline Acts::Surface& TelescopeDetectorElement::surface() {
  return *m_elementSurface;
}

inline double TelescopeDetectorElement::thickness() const {
  return m_elementThickness;
}

inline const Acts::Transform3& TelescopeDetectorElement::transform(
    const Acts::GeometryContext& gctx) const {
  // Check if a different transform than the nominal exists
  if (!m_alignedTransforms.empty()) {
    // cast into the right context object
    auto alignContext = gctx.get<ContextType>();
    return (*m_alignedTransforms[alignContext.iov].get());
  }
  // Return the standard transform if not found
  return nominalTransform(gctx);
}

inline const Acts::Transform3& TelescopeDetectorElement::nominalTransform(
    const Acts::GeometryContext& /*gctx*/) const {
  return *m_elementTransform;
}

#endif