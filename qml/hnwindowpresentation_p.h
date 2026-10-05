#pragma once
#include "hnwindowpresentation.h"

namespace Holonight::Private {
// Composition is separate from raw decoration negotiation.
inline HnWindowPresentation::State titleBarState(HnWindowDecoration::Mode mode, ExternalTitleBarState observation,
                                                 bool eligible) {
  if (!eligible) {
    return HnWindowPresentation::State::Unknown;
  }
  if (mode == HnWindowDecoration::Mode::ToolkitClientSide) {
    return HnWindowPresentation::State::Present;
  }
  if (mode == HnWindowDecoration::Mode::ServerSide) {
    return static_cast<HnWindowPresentation::State>(observation);
  }
  return HnWindowPresentation::State::Unknown;
}
}  // namespace Holonight::Private
