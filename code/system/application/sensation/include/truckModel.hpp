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

#ifndef TRUCKMODEL_HPP_
#define TRUCKMODEL_HPP_

#include "LinearizedSystemModel.hpp"


namespace opendlv {
namespace system {
namespace application {
namespace sensation{
namespace truckKinematicModel
{



/**
 * @brief System state vector-type for a 3DOF planar robot - including position and velocity
 *
 * This is a system state for a very simple planar robot that
 * is characterized by its (x,y)-Position and angular orientation.
 *
 * @param T Numeric scalar type
 */
template<typename T>
class State : public opendlv::system::libs::kalman::Vector<T, 6>
{
public:
    KALMAN_VECTOR(State, T, 6)    //the kalman vector for our state will be 6 (x, x_dot, y, y_dot, theta, theta_dot)

    //! X-position
    static constexpr size_t X = 0;
    //! X-Velocity
    static constexpr size_t X_dot = 1;

    //! Y-Position
    static constexpr size_t Y = 2;
    //! Y-Velocity
    static constexpr size_t Y_dot = 3;

    //! Orientation
    static constexpr size_t THETA = 4;
    //! Angular Velocity
    static constexpr size_t THETA_dot = 5;


    T x()       const { return (*this)[ X ]; }
    T x_dot()       const { return (*this)[ X_dot ]; }
    T y()       const { return (*this)[ Y ]; }
    T y_dot()       const { return (*this)[ Y_dot ]; }
    T theta()   const { return (*this)[ THETA ]; }
    T theta_dot()   const { return (*this)[ THETA_dot ]; }


    T& x()      { return (*this)[ X ]; }
    T& x_dot()      { return (*this)[ X_dot ]; }
    T& y()      { return (*this)[ Y ]; }
    T& y_dot()      { return (*this)[ Y_dot ]; }
    T& theta()  { return (*this)[ THETA ]; }
    T& theta_dot()  { return (*this)[ THETA_dot ]; }
};

/**
 * @brief System control-input vector-type for a 3DOF planar robot
 *
 * This is the system control-input of a very simple planar robot that
 * can control the velocity in its current direction as well as the
 * change in direction.
 *
 * @param T Numeric scalar type
 */
template<typename T>
class Control : public opendlv::system::libs::kalman::Vector<T, 2>
{
public:
    KALMAN_VECTOR(Control, T, 2)

    //! Velocity
    static constexpr size_t V = 0;
    //! Angular Rate (Orientation-change)
    static constexpr size_t DTHETA = 1;

    T v()       const { return (*this)[ V ]; }
    T dtheta()  const { return (*this)[ DTHETA ]; }

    T& v()      { return (*this)[ V ]; }
    T& dtheta() { return (*this)[ DTHETA ]; }
};

/**
 * @brief System model for a simple planar 3DOF robot
 *
 * This is the system model defining how our robot moves from one
 * time-step to the next, i.e. how the system state evolves over time.
 *
 * @param T Numeric scalar type
 * @param CovarianceBase Class template to determine the covariance representation
 *                       (as covariance matrix (StandardBase) or as lower-triangular
 *                       coveriace square root (SquareRootBase))
 */
template<typename T, template<class> class CovarianceBase = opendlv::system::libs::kalman::StandardBase>
class SystemModel : public opendlv::system::libs::kalman::LinearizedSystemModel<State<T>, Control<T>, CovarianceBase>
{
public:
    //! State type shortcut definition
    typedef State<T> S;

    //! Control type shortcut definition
    typedef Control<T> C;

    /**
     * @brief Definition of (non-linear) state transition function
     *
     * This function defines how the system state is propagated through time,
     * i.e. it defines in which state \f$\hat{x}_{k+1}\f$ is system is expected to
     * be in time-step \f$k+1\f$ given the current state \f$x_k\f$ in step \f$k\f$ and
     * the system control input \f$u\f$.
     *
     * @param [in] x The system state in current time-step
     * @param [in] u The control vector input
     * @returns The (predicted) system state in the next time-step
     */
    S f(const S& x, const C& u) const
    {
        //! Predicted state vector after transition
        S x_;

        // New orientation given by old orientation plus orientation change
        auto newOrientation = x.theta() + u.dtheta();
        // Re-scale orientation to [-pi/2 to +pi/2]

        x_.theta() = newOrientation;

        // New x-position given by old x-position plus change in x-direction
        // Change in x-direction is given by the cosine of the (new) orientation
        // times the velocity
        x_.x() = x.x() + std::cos( newOrientation ) * u.v();
        x_.y() = x.y() + std::sin( newOrientation ) * u.v();

        // Return transitioned state vector
        return x_;
    }


protected:
//TODO change the description of the truck model according to the real physic of the system

    /**
     * @brief Update jacobian matrices for the system state transition function using current state
     *
     * This will re-compute the (state-dependent) elements of the jacobian matrices
     * to linearize the non-linear state transition function \f$f(x,u)\f$ around the
     * current state \f$x\f$.
     *
     * @note This is only needed when implementing a LinearizedSystemModel,
     *       for usage with an ExtendedKalmanFilter or SquareRootExtendedKalmanFilter.
     *       When using a fully non-linear filter such as the UnscentedKalmanFilter
     *       or its square-root form then this is not needed.
     *
     * @param x The current system state around which to linearize
     * @param u The current system control input
     */
    void updateJacobians( const S& x, const C& u )
    {
        // F = df/dx (Jacobian of state transition w.r.t. the state)
        this->F.setZero();

        // partial derivative of x.x() w.r.t. x.x()
        this->F( S::X, S::X ) = 1;
        // partial derivative of x.x() w.r.t. x.theta()
        this->F( S::X, S::THETA ) = -std::sin( x.theta() + u.dtheta() ) * u.v();

        // partial derivative of x.y() w.r.t. x.y()
        this->F( S::Y, S::Y ) = 1;
        // partial derivative of x.y() w.r.t. x.theta()
        this->F( S::Y, S::THETA ) = std::cos( x.theta() + u.dtheta() ) * u.v();

        // partial derivative of x.theta() w.r.t. x.theta()
        this->F( S::THETA, S::THETA ) = 1;

        // W = df/dw (Jacobian of state transition w.r.t. the noise)
        this->W.setIdentity();
        // TODO: more sophisticated noise modelling
        //       i.e. The noise affects the the direction in which we move as
        //       well as the velocity (i.e. the distance we move)
    }
};
} // truckKinematicModel
} // Sensation
} // application
} // system
} // opendlv
#endif
