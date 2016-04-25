# Dockerfile - Dockerfile to create a Docker image from OpenDLV.
# Copyright (C) 2016 Christian Berger
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

# Date: 2016-03-05
# Docker Measurement REVERE

FROM ubuntu:14.04.4
MAINTAINER Magnus Thulin "magnus2lin@gmail.com"

RUN apt-get update -y
RUN apt-get upgrade -y

RUN apt-get -y --no-install-recommends install nano build-essential rpm cmake git ant 
RUN apt-get -y  install libcv-dev libhighgui-dev freeglut3 libqt4-dev libqwt5-qt4-dev libqwt5-qt4 libqt4-opengl-dev freeglut3-dev qt4-dev-tools libboost-dev libopencv-dev libopencv-photo-dev libopencv-contrib-dev openjdk-7-jdk default-jdk default-jre 

RUN mkdir /source


ADD packages /source
ADD opendlv /source

RUN dpkg -i /source/opendavinci_3.0.4_x86_64-lib.deb
RUN dpkg -i /source/opendavinci_3.0.4_x86_64-odsupercomponent.deb
RUN dpkg -i /source/opendavinci_3.0.4_x86_64-odCANDataStructureGenerator.deb
RUN dpkg -i /source/opendavinci_3.0.4_x86_64-odDataStructureGenerator.deb
RUN dpkg -i /source/opendavinci_3.0.4_x86_64-odcantools.deb
RUN dpkg -i /source/libautomotivedata_1.0.0_x86_64.deb
RUN dpkg -i /source/opendlv_3.0.4_x86_64-lib.deb

RUN cd /source && mkdir build && cd build && cmake .. && make

# Set locale (fix the locale warnings)
RUN localedef -v -c -i en_US -f UTF-8 en_US.UTF-8 || :

RUN cp /usr/local/configuration /usr/local/bin

ENV LD_LIBRARY_PATH /usr/local/lib/
WORKDIR /usr/local/bin
