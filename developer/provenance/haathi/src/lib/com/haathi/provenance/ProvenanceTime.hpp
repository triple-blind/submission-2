/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/ProvenanceTime.hpp#1 $

/*!
 @class ProvenanceTime
 This class defines a very basic time class. It has functions that can convert the time into 
 various parts.

 @author -- Aaron Gibson
 @date September 6, 2011
 */
/******************************************************************************
Guard
******************************************************************************/
#ifndef COM__HAATHI__PROVENANCE__PROVENANCE_TIME_HPP
#	define COM__HAATHI__PROVENANCE__PROVENANCE_TIME_HPP
/******************************************************************************
Include
******************************************************************************/
// We include the openoffice timer functions because they will help avoid some of the work.
// The functions defined in this include are C functions that will implicitly call the
// kernel. This isn't yet secure, but for now, it is sufficient.
#include "com/haathi/utils/StringWrapper.hpp"
#include "com/haathi/utils/Serialization.hpp"

#include "sdkBeg.hpp"
#include <osl/time.h>
#include "sdkEnd.hpp"

#include <iostream>
#include <string>
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
Class
******************************************************************************/
class ProvenanceTime {
	// For serialization
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_NVP(nanoseconds);
		ar & BOOST_SERIALIZATION_NVP(seconds);
		ar & BOOST_SERIALIZATION_NVP(minutes);
		ar & BOOST_SERIALIZATION_NVP(hours);
		ar & BOOST_SERIALIZATION_NVP(day);
		ar & BOOST_SERIALIZATION_NVP(dayOfWeek);
		ar & BOOST_SERIALIZATION_NVP(month);
		ar & BOOST_SERIALIZATION_NVP(year);
	}

	private:
		sal_uInt32 nanoseconds;
		sal_uInt8 seconds;
		sal_uInt8 minutes;
		sal_uInt8 hours;
		sal_uInt8 day;
		sal_uInt8 dayOfWeek;	// Sunday = 0
		sal_uInt8 month;	// 1-January
		sal_uInt16 year;
	public:
		// Constructors and Destructors
		ProvenanceTime();
		ProvenanceTime(oslDateTime dateTime);
		virtual ~ProvenanceTime() {
		}
		ProvenanceTime(const ProvenanceTime& other);
		ProvenanceTime& operator=(const ProvenanceTime& other);

		// Getters and Setters
		sal_uInt32 getNanoseconds() const {
			return nanoseconds;
		}
		sal_uInt8 getSeconds() const {
			return seconds;
		}
		sal_uInt8 getMinutes() const {
			return minutes;
		}
		sal_uInt8 getHours() const {
			return hours;
		}
		sal_uInt8 getDay() const {
			return day;
		}
		sal_uInt8 getDayOfWeek() const {
			return dayOfWeek;
		}
		sal_uInt8 getMonth() const {
			return month;
		}
		sal_uInt16 getYear() const {
			return year;
		}

		// String functions
		unsigned int getInYYYYMMDDFormat() const;
		unsigned int getInHHMMSSFormat() const;
		std::string toString() const;
		OUString toOUString() const;
		std::string printDate() const;
		std::string printTime() const;
		std::string printDateAndMillisecondsTime(bool convert = true) const;
		std::string printDateAndTime() const;
		friend std::ostream& operator<<(std::ostream& stream, const ProvenanceTime& time) {
			stream << time.toString();
			return stream;
		}

		// serialization
		std::string save() const;
		void load(std::string time);

		// Casting operator
		operator oslDateTime() const;

		// Static function
		static ProvenanceTime getCurrentTime();
};
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/******************************************************************************
Guard
******************************************************************************/
#endif
/*****************************************************************************/
