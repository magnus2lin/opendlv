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
 
#ifndef REMEMBEREDVEHICLE_HPP
#define REMEMBEREDVEHICLE_HPP


#include <iostream>
#include <vector>
#include <memory>

#include "detectvehicle/detectedvehicle.hpp"


// TODO add documentation

namespace opendlv {
namespace perception {
namespace detectvehicle {



 /**
 * @brief Class for handling vehicle detection.
 */
class RememberedVehicle {
public:
  RememberedVehicle();
  virtual ~RememberedVehicle();

/* Public methods */
public:
  void AddMemory(std::shared_ptr<DetectedVehicle> a_detectedVehiclePtr);
  void GetMemoryOverTime(std::vector<std::shared_ptr<DetectedVehicle>>* containerVector);
  std::shared_ptr<DetectedVehicle> GetLatestDetection();
  int32_t GetNrMemories();
  void CleanMemory(double timeStamp);
  cv::Scalar GetDummyColor();


/* Public fields */
public:

/* Private methods */
private:


/* Private fields */
private:
  std::shared_ptr<std::vector<std::shared_ptr<DetectedVehicle>>> m_memoryOverTime;
  cv::Scalar m_dummyColor;

};

} // detectvehicle
} // perception
} // opendlv


#endif
