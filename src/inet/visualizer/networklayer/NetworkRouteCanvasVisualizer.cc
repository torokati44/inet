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

#include "inet/visualizer/networklayer/NetworkRouteCanvasVisualizer.h"

#ifdef WITH_ETHERNET
#include "inet/linklayer/ethernet/switch/MACRelayUnit.h"
#endif

#ifdef WITH_IEEE8021D
#include "inet/linklayer/ieee8021d/relay/Ieee8021dRelay.h"
#endif

#ifdef WITH_IPv4
#include "inet/networklayer/ipv4/IPv4.h"
#endif

namespace inet {

namespace visualizer {

Define_Module(NetworkRouteCanvasVisualizer);

bool NetworkRouteCanvasVisualizer::isPathEnd(cModule *module) const
{
#ifdef WITH_IPv4
    if (dynamic_cast<IPv4 *>(module) != nullptr)
        return true;
#endif

    return false;
}

bool NetworkRouteCanvasVisualizer::isPathElement(cModule *module) const
{
#ifdef WITH_ETHERNET
    if (dynamic_cast<MACRelayUnit *>(module) != nullptr)
        return true;
#endif

#ifdef WITH_IEEE8021D
    if (dynamic_cast<Ieee8021dRelay *>(module) != nullptr)
        return true;
#endif

    return false;
}

const PathCanvasVisualizerBase::PathVisualization *NetworkRouteCanvasVisualizer::createPathVisualization(const std::vector<int>& path) const
{
    auto pathVisualization = static_cast<const PathCanvasVisualization *>(PathCanvasVisualizerBase::createPathVisualization(path));
    pathVisualization->figure->setTags("network_route");
    pathVisualization->figure->setTooltip("This path represents a network route between two network nodes");
    return pathVisualization;
}

} // namespace visualizer

} // namespace inet

