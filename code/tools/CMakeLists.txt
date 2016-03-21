# OpenDLV - Software for driverless vehicles realized with OpenDaVINCI
# Copyright (C) 2015 Chalmers ReVeRe
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

cmake_minimum_required(VERSION 2.8)

project(opendlv-tools)

###########################################################################
# Set the search path for .cmake files.
SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake.Modules" ${CMAKE_MODULE_PATH})
SET (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
SET (CMAKE_MODULE_PATH "${OPENDAVINCI_DIR}/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})

###########################################################################
# Compile flags.
if(UNIX)
set(COMPILER_FLAGS "-fstack-protector-all -fPIC -ggdb -Wall -Weffc++ -Werror -Wextra -Wshadow -Wswitch-default")
set(C_COMPILER_FLAGS "-std=c99")
set(CXX_COMPILER_FLAGS "-std=c++11")

set(CXX_EFFECTIVE_CXX " -Wmissing-format-attribute -Wredundant-decls -Wno-error=effc++ -Weffc++")
set(CXX_WARNING_ALL   " -Wno-deprecated -Wall -Werror -Wshadow -Wextra -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wpacked -Wcast-align -Wchar-subscripts -Wcomment -Wdisabled-optimization -Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wfloat-equal -Winit-self -Winline -Winvalid-pch -Wlong-long -Wsign-compare -Wuninitialized -Wunreachable-code -Wunsafe-loop-optimizations -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-but-set-parameter -Wunused-but-set-variable -Wunused-value -Wunused-variable -Wno-maybe-uninitialized -Wunused-result -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wparentheses -Wsign-compare -Wswitch -Wuninitialized -Wunknown-pragmas -Wunreachable-code -Wtrigraphs  -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wsync-nand -Wsuggest-attribute=const -Warray-bounds -Wtrampolines -Wlogical-op -Wnormalized=nfc -Wvarargs -Wvector-operation-performance -Wvla -Wtype-limits -Wc++11-compat -Woverloaded-virtual -Wcast-qual -Wctor-dtor-privacy -Wpacked -Wswitch-enum -Wswitch-default -Wdouble-promotion -Wmissing-declarations")
set(CXX_WARNING_TO_FIX " -Wno-error=conversion -Wno-error=useless-cast -Wno-error=old-style-cast -Wno-error=aggregate-return -Wno-error=zero-as-null-pointer-constant")
set(CXX_COMPILER_FLAGS "${CXX_COMPILER_FLAGS} ${CXX_EFFECTIVE_CXX} ${CXX_WARNING_ALL} ${CXX_WARNING_TO_FIX}")
endif()

set(CMAKE_C_FLAGS "${COMPILER_FLAGS} ${C_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS "${COMPILER_FLAGS} ${CXX_COMPILER_FLAGS}")

###########################################################################
# Find CxxTest.
include(CheckCxxTestEnvironment)

###########################################################################
# Find OpenDaVINCI.
find_package(OpenDaVINCI REQUIRED)

###########################################################################
# Find odcantools.
set(AUTOMOTIVEDATA_DIR "${OPENDAVINCI_DIR}")
set(ODCANTOOLS_DIR "${OPENDAVINCI_DIR}")
find_package(odcantools REQUIRED)

###########################################################################
# Find OpenDLV.
set(OPENDLV_DIR "${OPENDAVINCI_DIR}")
find_package(OpenDLV REQUIRED)

###########################################################################
# Find reverefh16mapping.
set(REVEREFH16MAPPING_DIR "${CMAKE_INSTALL_PREFIX}")
find_package(reverefh16mapping REQUIRED)

###########################################################################
# Find OpenDLV data structures.
set(OPENDLVDATA_DIR "${CMAKE_INSTALL_PREFIX}")
find_package(opendlvdata REQUIRED)

###########################################################################
# Set common include directories.
include_directories(${OPENDAVINCI_INCLUDE_DIRS})
include_directories(${OPENDLV_INCLUDE_DIRS})
include_directories(${ODCANTOOLS_INCLUDE_DIRS})
include_directories(${OPENDLVDATA_INCLUDE_DIRS})
include_directories(${REVEREFH16MAPPING_INCLUDE_DIRS})

###########################################################################
# Set libraries to link against.
set(LIBRARIES ${OPENDAVINCI_LIBRARIES}
              ${OPENDLV_LIBRARIES} 
              ${ODCANTOOLS_LIBRARIES}
              ${OPENDLVDATA_LIBRARIES}
              ${REVEREFH16MAPPING_LIBRARIES})

add_subdirectory(can)
add_subdirectory(vboreplay)
