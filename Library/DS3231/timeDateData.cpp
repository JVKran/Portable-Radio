/// @file

#include "hwlib.hpp"
#include "timeDateData.hpp"
/// \brief
/// Constructor
/// \details
/// This constructor has no mandatory parameters. Though the user can provide the hours, minutes and seconds to initialize
/// the ADT with those values.
timeData::timeData(const unsigned int givenHours, const unsigned int givenMinutes, const unsigned int givenSeconds):
	hours(givenHours),
	minutes(givenMinutes),
	seconds(givenSeconds)
{
	if(hours > 23){
		hours = 0;
	}
	if(minutes > 59){
		minutes = 0;
	}
	if(seconds > 59){
		seconds = 0;
	}
}

/// \brief
/// Constructor
/// \details
/// This constructor has one mandatory parameter; a timeData object. It copies the values of the given object.
timeData::timeData(const timeData & time){
	hours = time.hours;
	minutes = time.minutes;
	seconds = time.seconds;
}

/// \brief
/// Set Hours
/// \details
/// This function has one mandatory parameter; the current amount of hours. This function sets the hours to the
/// given amount.
void timeData::setHours(const unsigned int givenHours){
	if(givenHours < 24){
		hours = givenHours;
	}
}

/// \brief
/// Set Minutes
/// \details
/// This function has one mandatory parameter; the current amount of minutes. This function sets the minutes to the
/// given amount.
void timeData::setMinutes(const unsigned int givenMinutes){
	if(givenMinutes < 60){
		minutes = givenMinutes;
	}
}

/// \brief
/// Set Seconds
/// \details
/// This function has one mandatory parameter; the current amount of seconds. This function sets the seconds to the
/// given amount.
void timeData::setSeconds(const unsigned int givenSeconds){
	if(givenSeconds < 60){
		seconds = givenSeconds;
	}
}

/// \brief
/// Set Time
/// \details
/// This function has two mandatory parameters. The amount of hours and minutes. Most of the time the amount of seconds
/// is irrelevant; it defaults to 0. This function sets the amount of hours, minutes and seconds at once, instead of
/// one by one. Alternative to setHours(), setMinutes() and setSeconds() all together.
void timeData::setTime(const unsigned int givenHours, const unsigned int givenMinutes, const unsigned int givenSeconds){
	hours = givenHours;
	minutes = givenMinutes;
	seconds = givenSeconds;
	if(hours > 23){
		hours = 0;
	}
	if(minutes > 59){
		minutes = 0;
	}
	if(seconds > 59){
		seconds = 0;
	}
}

/// \brief
/// Set Time
/// \details
/// This function has one mandatory parameter; a timeData object. It copies the given values.
void timeData::setTime(const timeData & time){
	//Since the object given has already been checked for validity when it was created, we
	//do not have to do so again.
	hours = time.hours;
	minutes = time.minutes;
	seconds = time.seconds;
}

/// \brief
/// Get Hours
/// \details
/// This function returns the current set hours.
unsigned int timeData::getHours() const {
	return hours;
}

/// \brief
/// Get Minutes
/// \details
/// This function returns the current set minutes.
unsigned int timeData::getMinutes()const {
	return minutes;
}

/// \brief
/// Get Seconds
/// \details
/// This function returns the current set seconds.
unsigned int timeData::getSeconds()const {
	return seconds;
}

/// \brief
/// Operator+
/// \details
/// This function adds two timeData objects. Increasing of minutes and hours when respectively seconds and minutes
/// reach 60 is taken care of. When the amount of hours reaches 24 it is reset to 0.
timeData timeData::operator+(const timeData & rhs) const{
	timeData temp;
	temp = *this;
	temp += rhs;
	return temp;
}

/// \brief
/// Operator+=
/// \details
/// This function adds two timeData objects and stores the result in the object it is called on. Increasing of minutes 
/// and hours when respectively seconds and minutes reach 60 is taken care of. When the amount of hours reaches 24 it is reset to 0.
timeData & timeData::operator+=(const timeData & rhs){
	seconds += rhs.seconds;
	if(seconds > 59){
		minutes += (seconds / 60);
		seconds = (seconds % 60);
	}
	minutes += rhs.minutes;
	if(minutes > 59){
		hours += (minutes / 60);
		minutes = (minutes % 60);
	}
	hours += rhs.hours;
	if(hours > 24){
		hours = (hours % 24);
	}
	return *this;
}

/// \brief
/// Operator-
/// \details
/// This function substracts two timeData objects. Decreasing of minutes and hours when respectively seconds and minutes
/// reach 0 is taken care of. When the amount of hours reaches 0 it is reset to 23.
timeData timeData::operator-(const timeData & rhs) const{
	timeData temp;
	temp = *this;
	temp -= rhs;
	return temp;
}

/// \brief
/// Operator-=
/// \details
/// This function substracts two timeData objects and stores the result in the object it is called on. Decreasing of minutes 
/// and hours when respectively seconds and minutes reach 0 is taken care of. When the amount of hours reaches 0 it is reset to 23.
timeData & timeData::operator-=(const timeData & rhs){
	int negSeconds = seconds - rhs.seconds;
	if(negSeconds < -59){
		minutes -= (negSeconds / -60);
		seconds = (negSeconds % -60);
	} else {
		seconds -= rhs.seconds;
	}
	int negMinutes = minutes - rhs.minutes;
	if(negMinutes < -59){
		hours -= (negMinutes / -60);
		minutes = (negMinutes % -60);
	} else if(negMinutes < 0){
		minutes = 60 + negMinutes;
		hours--;
	} else {
		minutes -= rhs.minutes;
	}
	int negHours = hours - rhs.hours;
	if(negHours <= 0){
		hours = (24 + negHours);
		if(hours == 24){
			hours = 0;
		}
	} else {
		hours -= rhs.hours;
	}
	return *this;
}

/// \brief
/// Operator==
/// \details
/// This function compares two timeData objects and returns true if both objects contain exactly the same.
bool timeData::operator==(const timeData & rhs) const{
	return (hours == rhs.hours && minutes == rhs.minutes && seconds == rhs.seconds);
}

/// \brief
/// Operator!=
/// \details
/// This function compares two timeData objects and returns true if there is only a slightly difference
/// in content.
bool timeData::operator!=(const timeData & rhs) const{
	return (hours != rhs.hours || minutes != rhs.minutes || seconds != rhs.seconds);
}

/// \brief
/// Operator>
/// \details
/// This function returns true if the time object it is called upon is bigger than the compared object.
/// All attributes; hours, minutes and seconds are compared. When hours and minutes are the same, it is up 
/// to the seconds to determine which one is bigger.
bool timeData::operator>(const timeData & rhs) const{
	if(hours > rhs.hours){
		return true;
	} else if(hours == rhs.hours && minutes > rhs.minutes){
		return true;
	} else if (hours == rhs.hours && minutes == rhs.minutes && seconds > rhs.seconds){
		return true;
	} else {
		return false;
	}
}

/// \brief
/// Operator>=
/// \details
/// This function returns true if the time object it is called upon is bigger than or equal to the compared object.
/// All attributes; hours, minutes and seconds are compared. When hours and minutes are the same, it is up 
/// to the seconds to determine which one is bigger.
bool timeData::operator>=(const timeData & rhs) const{
	if (hours == rhs.hours && minutes == rhs.minutes && seconds >= rhs.seconds){
		return true;
	} else if(hours == rhs.hours && minutes >= rhs.minutes){
		return true;
	} else if(hours >= rhs.hours){
		return true;
	} else {
		return false;
	}
}

/// \brief
/// Operator<
/// \details
/// This function returns true if the time object it is called upon is smaller than the compared object.
/// All attributes; hours, minutes and seconds are compared. When hours and minutes are the same, it is up 
/// to the seconds to determine which one is bigger.
bool timeData::operator<(const timeData & rhs) const{
	if(hours < rhs.hours){
		return true;
	} else if(hours == rhs.hours && minutes < rhs.minutes){
		return true;
	} else if (hours == rhs.hours && minutes == rhs.minutes && seconds < rhs.seconds){
		return true;
	} else {
		return false;
	}
}

/// \brief
/// Operator<=
/// \details
/// This function returns true if the time object it is called upon is smaller than or equal to the compared object.
/// All attributes; hours, minutes and seconds are compared. When hours and minutes are the same, it is up 
/// to the seconds to determine which one is bigger.
bool timeData::operator<=(const timeData & rhs) const{
	if (hours == rhs.hours && minutes == rhs.minutes && seconds <= rhs.seconds){
		return true;
	} else if(hours == rhs.hours && minutes <= rhs.minutes){
		return true;
	} else if(hours <= rhs.hours){
		return true;
	} else {
		return false;
	}
}


//<<<------------------------------------------------------------------------------------------>>>

/// \brief
/// Constructor
/// \details
/// This constructor has no mandatory parameters. Though the user can provide the Day of Week, Day of Month, Month and year to
/// initialize the ADT with those values.
dateData::dateData(const unsigned int givenWeekDay, const unsigned int givenMonthDay, const unsigned int givenMonth, const unsigned int givenYear):
	weekDay(givenWeekDay),
	monthDay(givenMonthDay),
	month(givenMonth),
	year(givenYear)
{
	if(weekDay > 7 || weekDay < 1){
		weekDay = 1;
	}
	if(monthDay > 31 || monthDay < 1){
		monthDay = 1;
	}
	if(month > 12 || month < 1){
		month = 1;
	}
}

/// \brief
/// Constructor
/// \details
/// This constructor has one mandatory parameter; an object of type dateData. It copies the given values.
dateData::dateData(const dateData & date){
	//Since the object given has already been checked for validity when it was created, we
	//do not have to do so again.
	weekDay = date.weekDay;
	monthDay = date.monthDay;
	month = date.month;
	year = date.year;
}

/// \brief
/// Set Day of Week
/// \details
/// This constructor has one mandatory parameter; the day of week. This function sets the Day of Week
/// to the given amount.
void dateData::setWeekDay(const unsigned int givenWeekDay){
	if(givenWeekDay < 8 && givenWeekDay > 0){
		weekDay = givenWeekDay;
	}
}

/// \brief
/// Set Day of Month
/// \details
/// This constructor has one mandatory parameter; the day of Month. This function sets the Day of Month
/// to the given amount.
void dateData::setMonthDay(const unsigned int givenMonthDay){
	if(givenMonthDay < 32 && givenMonthDay > 0){
		monthDay = givenMonthDay;
	}
}

/// \brief
/// Set Month
/// \details
/// This constructor has one mandatory parameter; the Month. This function sets the Month
/// to the given amount.
void dateData::setMonth(const unsigned int givenMonth){
	if(givenMonth < 13 && givenMonth > 0){
		month = givenMonth;
	}
}

/// \brief
/// Set Year
/// \details
/// This constructor has one mandatory parameter; the year. This function sets the year
/// to the given amount.
void dateData::setYear(const unsigned int givenYear){
	year = givenYear;
}

/// \brief
/// Set Date
/// \details
/// This function has four mandatory parameters. The Day of Week, Day of Month, month and year. Instead
/// of setting these values one by one, which is the case with setWeekDay(), setMonthDay(), setMonth() and setYear(), 
/// this function sets these values all in one.
void dateData::setDate(const unsigned int givenWeekDay, const unsigned int givenMonthDay, const unsigned int givenMonth, const unsigned int givenYear){
	weekDay = givenWeekDay;
	monthDay = givenMonthDay;
	month = givenMonth;
	year = givenYear;
	if(weekDay > 7 || weekDay < 1){
		weekDay = 1;
	}
	if(monthDay > 31 || monthDay < 1){
		monthDay = 1;
	}
	if(month > 12 || month < 1){
		month = 1;
	}
}

/// \brief
/// Set Date
/// \details
/// This function has one mandatory parameter; a dateData object. It copies the given values.
void dateData::setDate(const dateData & date){
	//Since the object given has already been checked for validity when it was created, we
	//do not have to do so again.
	weekDay = date.weekDay;
	monthDay = date.monthDay;
	month = date.month;
	year = date.year;
}

/// \brief
/// Get Day of Week
/// \details
/// This function returns the currently set Day of Week.
unsigned int dateData::getWeekDay() const{
	return weekDay;
}

/// \brief
/// Get Day of Month
/// \details
/// This function returns the currently set Day of Month.
unsigned int dateData::getMonthDay() const{
	return monthDay;
}

/// \brief
/// Get Month
/// \details
/// This function returns the currently set Month.
unsigned int dateData::getMonth() const{
	return month;
}

/// \brief
/// Get Year
/// \details
/// This function returns the currently set Year.
unsigned int dateData::getYear() const{
	return year;
}

/// \brief
/// Operator+
/// \details
/// This adds two dateData objects. Increasing of month and year when respectively 30 and 12 are reached
/// is taken care of. However, this is a rough estimate.
dateData dateData::operator+(const dateData & rhs) const{
	dateData temp;
	temp = *this;
	temp += rhs;
	return temp;
}

/// \brief
/// Operator+=
/// \details
/// This adds two dateData objects and stores the result in the object it is called upon. Increasing of 
/// month and year when respectively 30 and 12 are reached is taken care of. However, this is a rough estimate 
/// since a month is assumed to always have 30 days.
dateData & dateData::operator+=(const dateData & rhs){
	if(weekDay + rhs.weekDay > 7){
		weekDay = (weekDay + rhs.weekDay) - 7;
	} else {
		weekDay += rhs.weekDay;
	}
	if(monthDay + rhs.monthDay > 30){
		month++;
		monthDay = (monthDay + rhs.monthDay) - 30;
	} else {
		monthDay += rhs.monthDay;
	}
	if(month + rhs.month > 12){
		year++;
		month = (month + rhs.month) - 12;
	} else {
		month += rhs.month;
	}
	year += rhs.year;
	return *this;
}

/// \brief
/// Operator-
/// \details
/// This substracts two dateData objects. Decreasing of month and year when respectively 0 and 0 are reached
/// is taken care of. However, this is a rough estimate since a month is assumed to always have 30 days.
dateData dateData::operator-(const dateData & rhs) const{
	dateData temp;
	temp = *this;
	temp -= rhs;
	return temp;
}

/// \brief
/// Operator-=
/// \details
/// This substracts two dateData objects and stores the result in the object it is called upon. Decreasing of month 
/// and year when respectively 0 and 0 are reached is taken care of. However, this is a rough estimate since a 
/// month is assumed to always have 30 days.
dateData & dateData::operator-=(const dateData & rhs){
	int negWeekDay = weekDay - rhs.weekDay;
	if(negWeekDay < 0){
		weekDay = negWeekDay + 7;
	} else {
		weekDay -= rhs.weekDay;
	}
	int negMonthDay = monthDay - rhs.monthDay;
	if(negMonthDay <= 0){
		monthDay = 30 + negMonthDay;
		month--;
	} else if (negMonthDay >= 0){
		monthDay -= rhs.monthDay;
	}
	int negMonth = month - rhs.month;
	if (negMonth > 0){
		month = negMonth;
	} else if (negMonth < 0){
		month = 12 + negMonth;
		year--;
	}
	int negYear = year - rhs.year;
	if(negYear < 0){
		year = 0;
	} else {
		year -= rhs.year;
	}
	return *this;
}

/// \brief
/// Operator==
/// \details
/// This function compares two dateData objects and returns true if both objects contain exactly the same.
bool dateData::operator==(const dateData & rhs) const{
	return (weekDay == rhs.weekDay && monthDay == rhs.monthDay && month == rhs.month && year == rhs.year);
}

/// \brief
/// Operator!=
/// \details
/// This function compares two dateData objects and returns true if there is only a slightly difference
/// in content.
bool dateData::operator!=(const dateData & rhs) const{
	return (weekDay != rhs.weekDay || monthDay != rhs.monthDay || month != rhs.month || year != rhs.year);
}

/// \brief
/// Operator<
/// \details
/// This function compares two dateData objects and returns true if the object it is called upon is smaller
/// than the object it is compared with. All attributes are compared; when year and month are the same, it is
/// up to the day of month to determine which is smallest.
bool dateData::operator<(const dateData & rhs) const{
	if(year < rhs.year){
		return true;
	} else if (year == rhs.year && month < rhs.month){
		return true;
	} else if (year == rhs.year && month == rhs.month && monthDay < rhs.monthDay){
		return true;
	} else {
		return false;
	}
}

/// \brief
/// Operator<=
/// \details
/// This function compares two dateData objects and returns true if the object it is called upon is smaller
/// than or equal to the object it is compared with. All attributes are compared; when year and month are the same, it is
/// up to the day of month to determine which is smallest.
bool dateData::operator<=(const dateData & rhs) const{
	if (year == rhs.year && month == rhs.month && monthDay <= rhs.monthDay){
		return true;
	} else if (year == rhs.year && month <= rhs.month){
		return true;
	} else if(year <= rhs.year){
		return true;
	} else {
		return false;
	}
}

/// \brief
/// Operator>
/// \details
/// This function compares two dateData objects and returns true if the object it is called upon is bigger
/// than the object it is compared with. All attributes are compared; when year and month are the same, it is
/// up to the day of month to determine which is smallest.
bool dateData::operator>(const dateData & rhs) const{
	if(year > rhs.year){
		return true;
	} else if (year == rhs.year && month > rhs.month){
		return true;
	} else if (year == rhs.year && month == rhs.month && monthDay > rhs.monthDay){
		return true;
	} else {
		return false;
	}
}

/// \brief
/// Operator>=
/// \details
/// This function compares two dateData objects and returns true if the object it is called upon is bigger
/// than or equal to the object it is compared with. All attributes are compared; when year and month are the same, it is
/// up to the day of month to determine which is smallest.
bool dateData::operator>=(const dateData & rhs) const{
	if (year == rhs.year && month == rhs.month && monthDay >= rhs.monthDay){
		return true;
	} else if (year == rhs.year && month >= rhs.month){
		return true;
	} else if(year >= rhs.year){
		return true;
	} else {
		return false;
	}
}

