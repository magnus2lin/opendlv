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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <chrono>
#include <fstream>
#include <iostream>

#include <opendavinci/odcore/data/Container.h>
#include <opendlv/data/environment/Point3.h>


#include "opendlvdata/GeneratedHeaders_opendlvdata.h"
#include "opendavinci/odcore/reflection/CSVFromVisitableVisitor.h"

#include "sensation.hpp"

namespace opendlv {
namespace system {
namespace application {
namespace sensation {



/**
  * Constructor.
  *
  * @param a_argc Number of command line arguments.
  * @param a_argv Command line arguments.
  */
Sensation::Sensation(int32_t const &a_argc, char **a_argv) :
    TimeTriggeredConferenceClientModule(a_argc, a_argv, "sensation"),
    X(),
    U(),
    sys(),
    Xdyn(),
    Udyn(),
    sys_dyn(),
    observationModel(0.0, 0.0,  0.0, 0.0 ), // clarify the numbers !
    dynObservationModel(0.0, 0.0,  0.0, 0.0 ),
    m_ekf(),
    m_dyn_ekf(),
    generator(),
    noise(0, 1),
    systemNoise(0),
    measurementNoise_x(0.05),
    measurementNoise_y(0.5),
    measurementNoise_yaw(0.001),
    measurementNoise_yawRate(0.0001),
    run_vse_test(false),
    m_saveToFile (false),
    EKF_initialized(false),
    m_timeBefore( ),
    m_timeNow( ),
    lastDataTime(0), // initialized to zero
    m_gpsReference(57.71278, opendlv::data::environment::WGS84Coordinate::NORTH,
                   11.94581583, opendlv::data::environment::WGS84Coordinate::EAST), // initialize to some coordinate in Sweden
    GPSreferenceSET(false)

{
    // Exit if the supercomponent is not running  --- should this statement be in all constructors (ACT-PERCEPTION ect.) to prevent possible errors ??
    //if (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::NOT_RUNNING)
    //{
    //    std::cout << getName() << " odSupercomponent is not running - run first " << std::endl;
    //    return ;
    //}

    initializeEKF();

}

Sensation::~Sensation()
{
}

void Sensation::setUp()
{
  // This method will be call automatically _before_ running body().
    // TODO should all the initializations go here ?? including ekf
    // initializeEKF();
}

void Sensation::tearDown()
{
  // This method will be call automatically _after_ return from body().
}





odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Sensation::body() {
    // Example on how to use the type WGS84Coordinate:

        using namespace opendlv::data::environment;


    // To dump data structures into a CSV file, you create an output file first.
    // std::ofstream fout("../Exp_data/output.csv");
    std::ofstream fout_ekfState("../Exp_data/output_ekf.csv");
    fout_ekfState << "%HEADER: Output of the Extended Kalman Filter, data format : \n"
                  << "%timestamp (s), ground truth: x (m),  y (m), theta (rad), theta_dot(rad/s), commands : velocity (m/s) steering angle (rad), noisy data: x (m), y (m), theta (rad), theta_dot (rad/s), ekf estimation vector: x (m), x_dot (m/s), y (m), y_dot (ms), theta (rad), theta_dot(rad/s)  \n"
                  << "%t lat long yaw long_vel wheels_angle Z_x Z_y Z_theta Z_theta_dot HAS_DATA X_x X_x_dot X_y X_y_dot X_theta X_theta_dot X_x_dyn X_x_dot_dyn X_y_dyn X_y_dot_dyn X_theta_dyn X_theta_dot_dyn " << endl;

    // You can optionally dump a header (i.e. first line with information).
    //const bool WITH_HEADER = true;
    // You can choose the delimiter character between the fields.
    //const char DELIMITER = ',';
    // For every data structure that you want to export in a CSV file, you need to create a new CSVFromVisitableVisitor.
    //odcore::reflection::CSVFromVisitableVisitor csvExporter1(fout, WITH_HEADER, DELIMITER);


    double deltaT = 0;
    double timeStamp = 0;

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {


        odcore::data::Container getPropulsionShaftVehicleSpeedData = getKeyValueDataStore().get(opendlv::proxy::reverefh16::Propulsion::ID());
        opendlv::proxy::reverefh16::Propulsion propulsionShaftVehicleSpeed  = getPropulsionShaftVehicleSpeedData.getData<opendlv::proxy::reverefh16::Propulsion>();

        odcore::data::Container getRoadwheelangleData = getKeyValueDataStore().get(opendlv::proxy::reverefh16::Steering::ID());
        opendlv::proxy::reverefh16::Steering roadwheelangle = getRoadwheelangleData.getData<opendlv::proxy::reverefh16::Steering>();

        odcore::data::Container gpsData = getKeyValueDataStore().get(opendlv::proxy::GpsReading::ID());
        opendlv::proxy::GpsReading gpsCoordinate = gpsData.getData<opendlv::proxy::GpsReading>();

       //uncomment for testing purposes
        /*cout << " I am receiving data from gps : "
            << "     latitude = " << gpsCoordinate.getLatitude() 
            << "     longitude= " << gpsCoordinate.getLongitude() 
            << "     heading =  " << gpsCoordinate.getNorthHeading() << endl;
       
       cout << " I am receiving data from can : "
            << "                velocity  = " << propulsionShaftVehicleSpeed.getPropulsionShaftVehicleSpeed()
            << "     steering wheel angle = " << roadwheelangle.getRoadwheelangle() << endl;
       */

        if (gpsData.getReceivedTimeStamp().toMicroseconds() > 0)//if we are actually getting data !
        {

        //this will run only ones to set the origin of the reference frame
            if ( !GPSreferenceSET )// the GPS reference is not set, set the gps reference to the current position
            {
            m_gpsReference = opendlv::data::environment::WGS84Coordinate( gpsData.getData<opendlv::proxy::GpsReading>().getLatitude(),  opendlv::data::environment::WGS84Coordinate::NORTH,
                                                                          gpsData.getData<opendlv::proxy::GpsReading>().getLongitude(), opendlv::data::environment::WGS84Coordinate::EAST);
            GPSreferenceSET = true;
            m_timeBefore = getPropulsionShaftVehicleSpeedData.getReceivedTimeStamp();
            // TODO: now this variable is set only ones using the first data,
            //       it is necessary to write a function able to reset this value to a new reference frame
            }


        m_timeNow = odcore::data::TimeStamp();
        odcore::data::TimeStamp duration = m_timeNow - m_timeBefore;
        //cout << getName() << ": <<message>> : time step in microseconds = " << duration.toMicroseconds() << endl;
        m_timeBefore = m_timeNow;
        deltaT = duration.toMicroseconds()/1000000.0;  //deltaT is in seconds

        // converts coordinates
        WGS84Coordinate WGS84_ptruck(gpsCoordinate.getLatitude(), WGS84Coordinate::NORTH, gpsCoordinate.getLongitude(), WGS84Coordinate::EAST);
        Point3 currentCartesianLocation = m_gpsReference.transform(WGS84_ptruck);

        // The csvExporter1 will "visit" the data structure "commands" and iterate
        // through its fields that will be stored in the output file fout.
        //commands.accept(csvExporter1);

         // set the commands from the opendavinci to the ekf state space
        if (getPropulsionShaftVehicleSpeedData.getReceivedTimeStamp().getSeconds() > 0) {//if we are actually getting data !
               U.v() = propulsionShaftVehicleSpeed.getPropulsionShaftVehicleSpeed();
               Udyn.v() = propulsionShaftVehicleSpeed.getPropulsionShaftVehicleSpeed();
        }
        else {
               U.v() = U.v(); // get the last imput data   -- find a policy for that
               Udyn.v() = Udyn.v();
        }

        if (getRoadwheelangleData.getReceivedTimeStamp().getSeconds() > 0) {//if we are actually getting data !
                U.phi() = roadwheelangle.getRoadwheelangle();
                Udyn.phi() = roadwheelangle.getRoadwheelangle();
         }
         else {
                U.phi() = U.phi(); // get the last imput data   -- find a policy for that
                Udyn.phi() = Udyn.phi();
        }

         // TODO handle IMU data
         Udyn.yaw_rate() = 0;//truckLocation.getYawRate();
         Udyn.v_y() = 0;// (truckLocation.getLat_acc() - Udyn.v_y())/deltaT;


         // System measurements
         m_tkmObservationVector Z = observationModel.h(X);
         m_tdmObservationVector Zdyn = dynObservationModel.h(Xdyn);

         // set the commands from the opendavinci to the ekf state space
         Z.Z_x()         =   currentCartesianLocation.getX();//truckLocation.getX();
         Z.Z_y()         =   currentCartesianLocation.getY();//truckLocation.getY();
         if (gpsCoordinate.getHasHeading())
            Z.Z_theta()     =   gpsCoordinate.getNorthHeading();
         else
             Z.Z_theta() = X.theta();

         Z.Z_theta_dot( )=  gpsCoordinate.getAltitude();// truckLocation.getYawRate();
         Zdyn.Z_x()         =   currentCartesianLocation.getX();//truckLocation.getX();
         Zdyn.Z_y()         =   currentCartesianLocation.getY();//truckLocation.getY();
         if (gpsCoordinate.getHasHeading())
            Zdyn.Z_theta()     =   gpsCoordinate.getNorthHeading();
         else
             Zdyn.Z_theta() = Xdyn.theta();

         Zdyn.Z_theta_dot( )=   gpsCoordinate.getAltitude();//truckLocation.getYawRate();
         //cout << getName() << " << message >> \n   MEASURES : " << " Z.Z_x()  = " << Z.Z_x() << " Z.Z_y()  = " << Z.Z_y()
         //                  << " Z.Z_theta()  = " << Z.Z_theta() << " Z.Z_theta_dot()  = " << Z.Z_theta_dot()  << endl;

         run_vse_test = false;
         if (run_vse_test) // if run test is true we are running a test and it will add noise to the measures
         {
             Z.Z_x() += measurementNoise_x * noise(generator);
             Z.Z_y() += measurementNoise_y * noise(generator);
             Z.Z_theta() += measurementNoise_yaw * noise(generator);
             Z.Z_theta_dot() += measurementNoise_yawRate * noise(generator);
         }


         if (EKF_initialized) // if the filter is not initialze - initialize it first
         {
             //std::cout << "Sensation::initializeEKF  << message >> Filter initialized " << std::endl;

             // update the timestamp - give the time in seconds
             sys.updateDeltaT(deltaT);
             sys_dyn.updateDeltaT(deltaT);
             std::cout << "Sensation::body  << message >> time updated " << sys.getDeltaT() << std::endl;
             std::cout << "Sensation::body  << message >> time updated " << sys_dyn.getDeltaT() << std::endl;

             // Predict state for current time-step using the filters
             X = m_ekf.predict(sys, U);
             Xdyn = m_dyn_ekf.predict(sys_dyn, Udyn);

            int hasData = 0;  // we now if the data we are saving to file is a pure prediction or a filtered data
            if (gpsData.getReceivedTimeStamp().toMicroseconds() > lastDataTime)
               {
                 // update stage of the EKF
                 X = m_ekf.update(observationModel, Z);
                 Xdyn = m_dyn_ekf.update(dynObservationModel, Zdyn);
                 hasData = 1;
                }
            else {
                hasData = 0;}

            // store the current instant for the next iteration
            lastDataTime = gpsData.getReceivedTimeStamp().toMicroseconds();

             timeStamp+=sys.getDeltaT();
           // Print to stdout
            std::cout   << getName() << " << message >> STATE \n"
                        << "timestamp = " << timeStamp << "\n"
                        << "           x " << X.x() << ", y " << X.y() << ", theta " << X.theta()  << "\n"
                        << "   dyn     x " << Xdyn.x() << ", y " << Xdyn.y() << ", theta " << Xdyn.theta()  << "\n"
                        << std::endl;

            //save data to file
            m_saveToFile = true;
            if (m_saveToFile){
            fout_ekfState << timeStamp << " "
                          << gpsCoordinate.getLatitude() << " " << gpsCoordinate.getLongitude() << " " << gpsCoordinate.getNorthHeading() <<  " "
                          << U.v() << " " << U.phi() << " "
                          << Z.Z_x() << " " << Z.Z_y() << " " << Z.Z_theta() << " " << Z.Z_theta_dot() << " " << hasData << " "
                          << X.x() << " " << X.x_dot() << " "  << X.y() << " " << X.y_dot() << " " << X.theta() << " " << X.theta_dot() << " "
                          << Xdyn.x() << " " << Xdyn.x_dot() << " " << Xdyn.y() << " "  << Xdyn.y_dot() << " " <<  Xdyn.theta() << " " << Xdyn.theta_dot()
                          << endl;
            }


            //TODO:: probably we need to send back the actual filtered GPS data including orientation - should I fill a container here?


         }
         else
         {
              std::cout << getName() << "  << message >> Filter NOT initialized " << std::endl;
         }

      }// end if we are getting data
        else {
            std::cout << getName() << " << message >> Filter -                                                             NO DATA ! " << std::endl;
        }
    }// end while
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}

void Sensation::initializeEKF()
{

if (!EKF_initialized)
{
    std::cout << "Sensation::initializeEKF  << message >> initialize the kalman filter " << std::endl;

    X.setZero();  // initialize the state vector
    Xdyn.setZero();
    generator.seed( std::chrono::system_clock::now().time_since_epoch().count() );


// all the necessary initializations should go here


    EKF_initialized = true;  // last operation if everything success
}
else
{
     std::cout << getName() << " << message >> Filter initialized " << std::endl;
}

}




} // sensation
} // application
} // system
} // opendlv




      /*
        cout << "[perception] received container of type " << c.getDataType() <<
                                               " sent at " << c.getSentTimeStamp().getYYYYMMDD_HHMMSSms() <<
                                           " received at " << c.getReceivedTimeStamp().getYYYYMMDD_HHMMSSms() << endl;

        if (c.getDataType() == Container::USER_DATA_2) {
            revere::generic::ExampleMessageForPerception payload = c.getData<revere::generic::ExampleMessageForPerception>();
            cout << "[perception]: " << payload.toString() << endl;

            // Example: Create message revere.generic.ExampleMessageForSafety.
            revere::generic::ExampleMessageForDecision nextMessage;
            nextMessage.setData1(5.2345);

            Container c2(Container::USER_DATA_3, nextMessage);
            getConference().send(c2);
        }
        */
