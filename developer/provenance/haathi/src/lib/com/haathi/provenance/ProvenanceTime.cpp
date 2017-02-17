/******************************************************************************
Header
******************************************************************************/
// $Header: //MyDataDepot/Projects/provenance-autoType2/haathi/src/lib/com/haathi/provenance/ProvenanceTime.cpp#1 $
/******************************************************************************
Include self
******************************************************************************/
#include "com/haathi/provenance/ProvenanceTime.hpp"
/******************************************************************************
Include others
******************************************************************************/
#include "com/haathi/exploit/Clock.hpp"
#include "com/haathi/utils/StringWrapper.hpp"

#include <string>
#include <sstream>
/******************************************************************************
Define
******************************************************************************/

/******************************************************************************
Using
******************************************************************************/
using com::haathi::utils::convertToOUString;
using com::haathi::utils::convertToString;
/******************************************************************************
Namespace
******************************************************************************/
namespace com {
	namespace haathi {
		namespace provenance {
/******************************************************************************
Class
******************************************************************************/
// Constructors and Destructors
ProvenanceTime::ProvenanceTime():
		nanoseconds(0), seconds(0), minutes(0), hours(0), day(0), dayOfWeek(0),
		month(0), year(0) {
}

// This is a tradeoff between readability, size, and portability
std::string ProvenanceTime::save() const {
	std::stringstream stream;

	stream
		<< (unsigned int) dayOfWeek << "-"
		<< (unsigned int) year << "/"
		<< (unsigned int) month << "/"
		<< (unsigned int) day << " "
		<< (unsigned int) hours << ":"
		<< (unsigned int) minutes << ":"
		<< (unsigned int) seconds << "."
		<< (unsigned int) nanoseconds;
	return stream.str();
}

int loadUpTo(std::string time, int* pos, char end) {
	int value = 0;

	for (const char* c = time.c_str(); c[*pos] != end; (*pos)++)
		value = value * 10 + (c[*pos] - '0');
	(*pos)++;
	return value;
}

void ProvenanceTime::load(std::string time) {
	int pos = 0;

	dayOfWeek = loadUpTo(time, &pos, '-');
	year = loadUpTo(time, &pos, '/');
	month = loadUpTo(time, &pos, '/');
	day = loadUpTo(time, &pos, ' ');
	hours = loadUpTo(time, &pos, ':');
	minutes = loadUpTo(time, &pos, ':');
	seconds = loadUpTo(time, &pos, '.');
	nanoseconds = loadUpTo(time, &pos, '\0');
}

ProvenanceTime::ProvenanceTime(oslDateTime date): nanoseconds(date.NanoSeconds),
		seconds((sal_uInt8) date.Seconds), minutes((sal_uInt8) date.Minutes), hours((sal_uInt8) date.Hours),
		day((sal_uInt8) date.Day), dayOfWeek((sal_uInt8) date.DayOfWeek), month((sal_uInt8) date.Month),
		year(date.Year) {
	// Nothing to do here.
}

ProvenanceTime::ProvenanceTime(const ProvenanceTime& other):
		nanoseconds(other.nanoseconds), seconds(other.seconds), minutes(
				other.minutes), hours(other.hours), day(other.day), dayOfWeek(
				other.dayOfWeek), month(other.month), year(other.year) {
	// Nothing to do here.
}

ProvenanceTime& ProvenanceTime::operator=(const ProvenanceTime& other) {
	nanoseconds = other.nanoseconds;
	seconds = other.seconds;
	minutes = other.minutes;
	hours = other.hours;
	day = other.day;
	dayOfWeek = other.dayOfWeek;
	month = other.month;
	year = other.year;
	return *this;
}

std::string ProvenanceTime::toString() const {
	// In the code below, you may wonder why I explicitly invoke the 
	// templated version of convertToString with the sal_uInt32 datatype.
	// This is because some of these parameters are sal_uInt8 datatypes, which
	// is more commonly known as an 'unsigned char'. Due to the nature of the 
	// function, we do NOT want the program to assume that the parameters are 
	// chars because the program will try and print random ascii characters 
	// instead of the actual number that we care about.
	std::string result;
	// First, print the date.
	result += "Date: ";
	result += convertToString<sal_uInt32>(month);
	result += "/";
	result += convertToString<sal_uInt32>(day);
	result += "/";
	result += convertToString<sal_uInt32>(year);
	result += " ";

	// Now for the time.
	result += convertToString<sal_uInt32>(hours);
	result += ":";
	result += convertToString<sal_uInt32>(minutes);
	result += " Seconds: ";
	result += convertToString<sal_uInt32>(seconds);
	result += " Nanoseconds: ";
	result += convertToString<sal_uInt32>(nanoseconds);

	return result;
}

unsigned int ProvenanceTime::getInHHMMSSFormat() const {
	unsigned int result = 0;

	result += hours * 10000;
	result += minutes * 100;
	result += seconds;

	return result;
}

unsigned int ProvenanceTime::getInYYYYMMDDFormat() const {
	unsigned int result = 0;

	result += year * 10000;
	result += month * 100;
	result += day;

	return result;
}

OUString ProvenanceTime::toOUString() const {
	return convertToOUString(toString().c_str());
}

std::string ProvenanceTime::printDateAndMillisecondsTime(bool convert) const {
	if (convert) {
		// Times are stored GTM and need to be converted for display
		oslDateTime systemDateTime = (oslDateTime) *this;

		TimeValue systemTimeValue;
		osl_getTimeValueFromDateTime(&systemDateTime, &systemTimeValue);

		TimeValue localTimeValue;
		osl_getLocalTimeFromSystemTime(&systemTimeValue, &localTimeValue);

		oslDateTime localDateTime;
		osl_getDateTimeFromTimeValue(&localTimeValue, &localDateTime);

		ProvenanceTime localProvenanceTime(localDateTime);

		return localProvenanceTime.printDateAndMillisecondsTime(false);
	}

	sal_uInt32 tmpNanoseconds = nanoseconds;
	std::stringstream result;

	result << printDateAndTime() << ":";
	if (seconds < 10)
		result << "0";
	result << convertToString<sal_uInt32>(seconds) << ".";

	for (int div = 100000000; div >= 1; div /= 10) {
		result << (char) ('0' + (tmpNanoseconds / div));
		tmpNanoseconds %= div;
	}
	return result.str();
}

std::string ProvenanceTime::printDate() const {
	std::string result;
	switch (month) {
	case 1:
		result += "January";
		break;
	case 2:
		result += "February";
		break;
	case 3:
		result += "March";
		break;
	case 4:
		result += "April";
		break;
	case 5:
		result += "May";
		break;
	case 6:
		result += "June";
		break;
	case 7:
		result += "July";
		break;
	case 8:
		result += "August";
		break;
	case 9:
		result += "September";
		break;
	case 10:
		result += "October";
		break;
	case 11:
		result += "November";
		break;
	case 12:
		result += "December";
		break;
//		default:
		// Should never get here...
	}
	result += " ";
	result += convertToString<sal_uInt32>(day);
	result += ", ";
	result += convertToString<sal_uInt32>(year);

	return result;
}
std::string ProvenanceTime::printTime() const {
	std::string result;
	result += convertToString<sal_uInt32>(hours);
	result += ":";

	if (minutes < 10) {
		result += "0";
	}
	result += convertToString<sal_uInt32>(minutes);

	return result;
}

std::string ProvenanceTime::printDateAndTime() const {
	return (printDate() + " " + printTime());
}

ProvenanceTime::operator oslDateTime() const {
	oslDateTime result;

	result.NanoSeconds = nanoseconds;
	result.Seconds = seconds;
	result.Minutes = minutes;
	result.Hours = hours;
	result.Day = day;
	result.DayOfWeek = dayOfWeek;
	result.Month = month;
	result.Year = year;
	return result;
}

// Static function
ProvenanceTime ProvenanceTime::getCurrentTime() {
	TimeValue time_value;
	oslDateTime osl_time;

	osl_getSystemTime(&time_value);
#if EXPLOIT == EXPLOIT_CLOCK
	com::haathi::exploit::clock.captureTimeValue(&time_value);
#endif
	osl_getDateTimeFromTimeValue(&time_value, &osl_time);
	return ProvenanceTime(osl_time);
}
/******************************************************************************
Namespace
******************************************************************************/
		};
	};
};
/*****************************************************************************/
