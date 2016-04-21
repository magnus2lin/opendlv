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

#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/data/TimeStamp.h"

#include "opendlvdata/GeneratedHeaders_opendlvdata.h"

#include "act/act.hpp"

 namespace opendlv {
  namespace action {
    namespace act {

/**
  * Constructor.
  *
  * @param a_argc Number of command line arguments.
  * @param a_argv Command line arguments.
  */
  Act::Act(int32_t const &a_argc, char **a_argv)
  : TimeTriggeredConferenceClientModule(a_argc, a_argv, "action-act"),
  m_acceleration(-2.0f),
  m_steering(0.0f),
  deltaTime(0.5f),
  startTimeVectorAccelerate[],
  startTimeVectorBrake[],
  startTimeVectorSteering[];
  amplitudeVectorAccelerate[],
  amplitudeVectorBrake[],
  amplitudeVectorSteering[],
  counterAccelerate(0),
  counterBrake(0),
  counterSteering(0)
  {
  }

  Act::~Act()
  {
  }

/**
 * Receives control correction requests, including a modality, if inhibitory,
 * amplitude, and a start time.
 * Sends modulated contol signal as individual samples, per modality to Proxy
 * actuators.
 */
 odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Act::body()
 {
  while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
    odcore::data::dmcp::ModuleStateMessage::RUNNING) {

  //  std::cout << "Send acc. " << m_acceleration << " Steering: " << m_steering << std::endl;

    opendlv::proxy::Actuation actuation(m_acceleration, m_steering, false);
  odcore::data::Container c(actuation);
  getConference().send(c);
}

return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

void Act::nextContainer(odcore::data::Container &c)
{
  if(c.getDataType() == opendlv::action::Correction::ID()) {
    opendlv::action::Correction correction = 
    c.getData<opendlv::action::Correction>();

    odcore::data::TimeStamp t0 = correction.getStartTime();
    std::string type = correction.getType();
    bool isInhibitory = correction.getIsInhibitory();
    float amplitude = correction.getAmplitude();

    if (type == "accelerate") {
      inhibitoryCheck(isInhibitory, startTimeVectorAccelerate[], amplitudeVectorAccelerate[], counterAccelerate)
      startTimeVectorAccelerate[counter] = t0;
      amplitudeVectorAccelerate[counter] = amplitude;
      counterAccelerate++;

      if (odcore::data::TimeStamp timeStamp - startTimeVectorAccelerate[0] <= 0 ) {
        for (int counter = 0; counter < counterAccelerate; counter++) {
          startTimeVectorAccelerate[counter] = startTimeVectorAccelerate[counter+1]
          amplitudeVectorAccelerate[counter] = amplitudeVectorAccelerate[counter+1]
        }
      }
    }

    else if (type == "brake") {
      inhibitoryCheck(isInhibitory, startTimeVectorBrake[], amplitudeVectorBrake[], counterBrake)
      startTimeVectorBrake[counter] = t0;
      amplitudeVectorBrake[counter] = amplitude;
      counterBrake++;
    }

    else if (type == "steering") {
      inhibitoryCheck(isInhibitory, startTimeVectorSteering[], amplitudeVectorSteering[], counterSteering)
      startTimeVectorSteering[counter] = t0;
      amplitudeVectorSteering[counter] = amplitude;
      counterAccelerate++;
    }











    /*if (type == "accelerate") {
      //std::cout << "accelerate: " << amplitude << std::endl;
      m_acceleration = amplitude;
    } else if (type == "brake") {
      std::cout << "brake: " << amplitude << std::endl;
    } else if (type == "steering") {
      std::cout << "steering: " << amplitude << std::endl;
    }*/

    }
  }


  void Act::timeCheck(float &timeVector, float &amplitudeVector, uint32_t counter)
  {
    if (odcore::data::TimeStamp timeStamp - timeVector[0] <= 0 ) {
      for (uint32_t count = 0; count < counterAccelerate; count++) {
        timeVector[count] = timeVector[count+1]
        amplitudeVector[count] = amplitudeVector[count+1]
      }
    }
  }

  int Act::inhibitoryCheck(bool isInhibitory, float &timeVector[], float &amplitudeVector[], uint32_t counter )
  {
    if (isInhibitory) {
      timeVector.clear;
      amplitudeVector.clear;
      counter = 0;
    }
    return counter;
  }

  void Act::setUp()
  {
  }

  void Act::tearDown()
  {
  }

} // act
} // action
} // opendlv
