#ifndef TRACK_HH
#define TRACK_HH
#include "Acts/EventData/TrackContainer.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"
#include "Acts/EventData/VectorTrackContainer.hpp"
#include <vector>

/// (Reconstructed) track parameters e.g. close to the vertex.
using TrackParameters = ::Acts::BoundTrackParameters;
/// Container of reconstructed track states for multiple tracks.
using TrackParametersContainer = std::vector<TrackParameters>;

using TrackContainer =
    Acts::TrackContainer<Acts::VectorTrackContainer,
                         Acts::VectorMultiTrajectory, std::shared_ptr>;

using ConstTrackContainer =
    Acts::TrackContainer<Acts::ConstVectorTrackContainer,
                         Acts::ConstVectorMultiTrajectory, std::shared_ptr>;


#endif