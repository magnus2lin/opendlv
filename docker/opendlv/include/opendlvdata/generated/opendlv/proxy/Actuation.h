/*
 * This software is open source. Please see COPYING and AUTHORS for further information.
 *
 * This file is auto-generated. DO NOT CHANGE AS YOUR CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef OPENDLV_PROXY_ACTUATION_H
#define OPENDLV_PROXY_ACTUATION_H

#include "opendavinci/odcore/opendavinci.h"


#include "opendavinci/odcore/base/Visitable.h"
#include "opendavinci/odcore/data/SerializableData.h"


namespace opendlv {
	namespace proxy {
		using namespace std;
		
		class Actuation : public odcore::data::SerializableData, public odcore::base::Visitable {
			public:
				Actuation();
		
				Actuation(
				const float &val0, 
				const float &val1, 
				const bool &val2
				);
		
				virtual ~Actuation();
		
				/**
				 * Copy constructor.
				 *
				 * @param obj Reference to an object of this class.
				 */
				Actuation(const Actuation &obj);
		
				/**
				 * Assignment operator.
				 *
				 * @param obj Reference to an object of this class.
				 * @return Reference to this instance.
				 */
				Actuation& operator=(const Actuation &obj);
		
			public:
				/**
				 * @return acceleration.
				 */
				float getAcceleration() const;
				
				/**
				 * This method sets acceleration.
				 *
				 * @param val Value for acceleration.
				 */
				void setAcceleration(const float &val);
			public:
				/**
				 * @return steering.
				 */
				float getSteering() const;
				
				/**
				 * This method sets steering.
				 *
				 * @param val Value for steering.
				 */
				void setSteering(const float &val);
			public:
				/**
				 * @return isValid.
				 */
				bool getIsValid() const;
				
				/**
				 * This method sets isValid.
				 *
				 * @param val Value for isValid.
				 */
				void setIsValid(const bool &val);
		
			public:
				/**
				 * This method returns the message id.
				 *
				 * @return Message id.
				 */
				static int32_t ID();
		
				/**
				 * This method returns the short message name.
				 *
				 * @return Short message name.
				 */
				static const string ShortName();
		
				/**
				 * This method returns the long message name include package/sub structure.
				 *
				 * @return Long message name.
				 */
				static const string LongName();
		
				/**
				 * This method returns the message id.
				 *
				 * @return Message id.
				 */
				virtual int32_t getID() const;
		
				/**
				 * This method returns the short message name.
				 *
				 * @return Short message name.
				 */
				virtual const string getShortName() const;
		
				/**
				 * This method returns the long message name include package/sub structure.
				 *
				 * @return Long message name.
				 */
				virtual const string getLongName() const;
		
			public:
				virtual void accept(odcore::base::Visitor &v);
		
				virtual ostream& operator<<(ostream &out) const;
				virtual istream& operator>>(istream &in);
		
				virtual const string toString() const;
		
			private:
				float m_acceleration;
			private:
				float m_steering;
			private:
				bool m_isValid;
		
		};
	} // proxy
} // opendlv

#endif /*OPENDLV_PROXY_ACTUATION_H*/