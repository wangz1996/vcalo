#ifndef SIMHIT_HH
#define SIMHIT_HH
#include "GeometryContainers.hpp"
#include "ActsFatras/EventData/Hit.hpp"

using SimBarcode = ::ActsFatras::Barcode;

using SimHit = ::ActsFatras::Hit;
/// Store hits ordered by geometry identifier.
using SimHitContainer = GeometryIdMultiset<::ActsFatras::Hit>;

#endif