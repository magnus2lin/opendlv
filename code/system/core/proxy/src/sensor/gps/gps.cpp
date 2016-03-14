/**
 * Copyright (C) 2015 Chalmers REVERE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include <ctype.h>
#include <cstring>
#include <cmath>
#include <iostream>

#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/data/TimeStamp.h"

#include "opendlv/data/sensor/nmea/GPRMC.h"
#include "opendlv/data/environment/WGS84Coordinate.h"

#include "opendlvdata/GeneratedHeaders_OpenDLVData.h"

#include "sensor/gps/gps.hpp"
#include "sensor/gps/device.hpp"

namespace opendlv {
namespace proxy {
namespace sensor {
namespace gps {

/**
  * Constructor.
  *
  * @param a_argc Number of command line arguments.
  * @param a_argv Command line arguments.
  */
Gps::Gps(int32_t const &a_argc, char **a_argv) :
    TimeTriggeredConferenceClientModule(a_argc, a_argv, "proxy-sensor-gps"),
    m_device()
{
}

Gps::~Gps() 
{
}

// This method will do the main data processing job.
odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Gps::body()
{
  return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

void Gps::setUp() 
{
  odcore::base::KeyValueConfiguration kv = getKeyValueConfiguration();

  std::string const type = kv.getValue<std::string>("proxy-sensor-gps.type");
/*  std::string const port = kv.getValue<std::string>("proxy-sensor-gps.port");
  float const mountX = kv.getValue<float>("proxy-sensor-gps.mount.x");
  float const mountY = kv.getValue<float>("proxy-sensor-gps.mount.y");
  float const mountZ = kv.getValue<float>("proxy-sensor-gps.mount.z");
*/
  if (type.compare("trimble") == 0) {
//      m_device = std::unique_ptr<Device>(new TrimbleDevice());

  // Example on how to decode and work with GPRMC data streams.
  {
    using namespace odcore::data;
    using namespace opendlv::data::environment;
    using namespace opendlv::data::sensor::nmea;

    // Value of test coordinate.
    double latitude = 52.247041;
    double longitude = 10.575830;

    WGS84Coordinate testCoordinate(latitude, WGS84Coordinate::NORTH, longitude, WGS84Coordinate::EAST);

    // Imaginary time stamp when the GPRMC string was received in our software.
    TimeStamp ts(1240926174, 0);

    // Example $GPRMC string received from the GPS unit.
    stringstream example;
    example << "$GPRMC,134254,A,5214.8225,N,01034.5498,E,0.0,0.0,280409,0.0,E,S*c" << endl;

    // GPRMC is a class that can extract data from a GPRMC string as provided by a GPS receiver.
    GPRMC gprmc;
    gprmc.setMessage(example.str());

    std::cout << "testCoordinate: lat = " << testCoordinate.getLatitude() << ", lon = " << testCoordinate.getLatitude() << std::endl;
    std::cout << "decoded via GPRMC: lat = " << gprmc.getCoordinate().getLatitude() << ", lon = " << gprmc.getCoordinate().getLatitude() << std::endl;
  }
    
  }

  if (m_device.get() == nullptr) {
    std::cerr << "[proxy-sensor-gps] No valid device driver defined." 
        << std::endl;
  }
}

void Gps::tearDown() 
{
}

} // gps
} // sensor
} // proxy
} // opendlv
