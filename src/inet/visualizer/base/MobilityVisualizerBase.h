//
// Copyright (C) OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_MOBILITYVISUALIZERBASE_H
#define __INET_MOBILITYVISUALIZERBASE_H

#include "inet/mobility/contract/IMobility.h"
#include "inet/visualizer/base/VisualizerBase.h"

namespace inet {

namespace visualizer {

class INET_API MobilityVisualizerBase : public VisualizerBase, public cListener
{
  protected:
    class INET_API MobilityVisualization {
      public:
        IMobility *mobility = nullptr;

      public:
        MobilityVisualization(IMobility *mobility);
    };
  protected:
    /** @name Parameters */
    //@{
    cModule *subscriptionModule = nullptr;
    // orientation
    bool displayOrientation = false;
    double orientationArcSize = NaN;
    cFigure::Color orientationLineColor;
    double orientationLineWidth = NaN;
    // velocity
    bool displayVelocity = false;
    double velocityArrowScale = NaN;
    cFigure::Color velocityLineColor;
    double velocityLineWidth = NaN;
    cFigure::LineStyle velocityLineStyle;
    // movement trail
    bool displayMovementTrail = false;
    bool autoMovementTrailLineColor = false;
    cFigure::Color movementTrailLineColor;
    double movementTrailLineWidth = NaN;
    int trailLength = -1;
    //@}

  protected:
    virtual void initialize(int stage) override;

  public:
    virtual ~MobilityVisualizerBase();
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_MOBILITYVISUALIZERBASE_H

