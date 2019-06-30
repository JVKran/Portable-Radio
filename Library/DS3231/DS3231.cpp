/// @file

#include "hwlib.hpp"
#include "DS3231.hpp"

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
	setHours(givenHours);
	setMinutes(givenMinutes);
	setSeconds(givenSeconds);
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
	} else {
		minutes -= rhs.minutes;
	}
	int negHours = hours - rhs.hours;
	if(negHours < 0){
		hours = (24 + negHours);
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
		weekDay = 7;
	}
	if(monthDay > 31 || monthDay < 1){
		monthDay = 31;
	}
	if(month > 12 || month < 1){
		month = 12;
	}
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
	setWeekDay(givenWeekDay);
	setMonthDay(givenMonthDay);
	setMonth(givenMonth);
	setYear(givenYear);
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
	if(negMonthDay < 0){
		monthDay = negMonthDay * -1;
		month--;
	} else if (negMonthDay > 0){
		monthDay = negMonthDay;
		month++;
	} else {
		monthDay += rhs.monthDay;
	}
	int negMonth = month - rhs.month;
	if(negMonth > 12){
		month = 12 + negMonth;
		year--;
	} else if(negMonth > 0){
		month = negMonth;
	} else if (negMonth < 0){
		month = 12 + negMonth;
	} else {
		month -= rhs.month;
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

//<<<------------------------------------------------------------------------------------------->>>

/// \brief
/// Constructor
/// \details
/// This constructor has no mandatory parameters. Though the user can provide the matchconditions, as specified
/// in the datasheet, and wether or not he wants to have the DS3231 to output a high signal at (SKW) when the alarm
/// triggers.
alarm::alarm(const unsigned int matchConditions, const bool outputSignal):
	matchConditions(matchConditions),
	outputSignal(outputSignal),
	time(timeData()),
	date(dateData())
{}

/// \brief
/// Set Match Conditions
/// \details
/// This function has one mandatory parameter; the match condition the alarm has to meet before it gets triggered.
void alarm::setMatchConditions(const unsigned int condition){
	matchConditions = condition;
}

/// \brief
/// Get Match Conditions
/// \details
/// This function returns the currently set match conditions before the alarm can trigger.
unsigned int alarm::getMatchConditions(){
	return matchConditions;
}

/// \brief
/// Set Outputsignal
/// \details
/// This function has one mandatory parameter; wether to set or unset the outputsignal when the alarm meets
/// the match-conditions.
void alarm::enableOutputSignal(const bool enable){
	outputSignal = enable;
}

//<<<------------------------------------------------------------------------------------------->>>

/// \brief
/// Constructor
/// \details
/// This constructor has one mandatory parameter; the I2C bus. The user can also provide the address. 
/// The address defaults to 0x68.
DS3231::DS3231(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address):
	bus(bus),
	address(address),
	firstAlarm(alarm()),
	secondAlarm(alarm()),
	time(timeData()),
	date(dateData())
{}

/// \brief
/// Get I2C Address
/// \details
/// This function returns the currently set address with which the chip has been initialized.
unsigned int DS3231::getAddress(){
	return address;
}

/// \brief
/// Send Data
/// \details
/// This function sends all available data (the entire data array) to the DS3231. Not used very much since
/// it also overwrites the kept hours, minutes, seconds, day, month and year.
void DS3231::setData(){
	auto transaction = bus.write(address);
	transaction.write(0x00);
	transaction.write(data, 8);
}

/// \brief
/// Get Status
/// \details
/// This function gets and stores all neccesary data from the DS3231 to retrieve the time and date.
void DS3231::getStatus(){
	bus.write(address).write(0x00);
	auto transaction = bus.read(address);
	transaction.read(status, 7);
	hwlib::wait_ms(30);

	time.setSeconds((status[0] & 0x0F) + ((status[0] >> 4) & 0x07) * 10);
	time.setMinutes(((status[1] & 0x0F) + ((status[1] >> 4) & 0x07) * 10));
	time.setHours((status[2] & 0x0F) + ((status[2] >> 4) * 10) + (status[2] >> 5) * 20);

	date.setWeekDay(status[3] & 0x07);
	date.setMonthDay((status[4] & 0x0F) + (((status[4] >> 4) & 0x03) * 10));
	date.setMonth(((status[5] & 0x0F) + ((status[5] >> 4) & 0x01) * 10));
	date.setYear(((status[6] & 0x0F) + (((status[6] >> 4) & 0x0F) * 10) + (2000 - (100 * (status[5] >> 7 & 1)))));
}

/// \brief
/// Set Time
/// \details
/// This function has two mandatory parameters; the hours and minutes. The amount of seconds is optional since
/// this precision is mostly not necessary or practical. Though the user can specify it. The specified data will be written
/// to the DS3231, from which it will keep track of the day, month and year.
void DS3231::setTime(const unsigned int hours, const unsigned int minutes, const unsigned int seconds){
	time.setHours(hours);
	time.setMinutes(minutes);
	time.setSeconds(seconds);
	auto transaction = bus.write(address);
	transaction.write(0x00);
	transaction.write((((seconds / 10) & 0x07) << 4) + ((seconds % 10) & 0x0F));
	transaction.write((((minutes / 10) & 0x07) << 4) + ((minutes % 10) & 0x0F));
	transaction.write((((hours / 10) & 0x01) << 4) + ((hours % 10) & 0x0F));
}

/// \brief
/// Set Data
/// \details
/// This function has four mandatory parameters; day of week, day of month, month and year. The specified data will be written
/// to the DS3231, from which it will keep track of the day, month and year.
void DS3231::setDate(const unsigned int weekDay, const unsigned int monthDay, const unsigned int month, const unsigned int year){
	date.setWeekDay(weekDay);
	date.setMonthDay(monthDay);
	date.setMonth(month);
	date.setYear(year);
	auto transaction = bus.write(address);
	transaction.write(0x03);
	transaction.write(weekDay & 0x07);
	transaction.write((((monthDay / 10) & 0x03) << 4) + ((monthDay % 10) & 0x0F));
	if(year < 2000){
		transaction.write((((month / 10) & 0x01) << 4) + ((month % 10) & 0x0F) + (1UL << 7));
		transaction.write(((((year - 1900) / 10) << 4) & 0xF0) + (((year - 1900) % 10) & 0x0F));
	} else {
		transaction.write((((month / 10) & 0x01) << 4) + ((month % 10) & 0x0F));
		transaction.write(((((year - 2000) / 10) << 4) & 0xF0) + (((year - 2000) % 10) & 0x0F));
	}
}

/// \brief
/// Get Time
/// \details
/// This function returns the current time as an object of type timeData. The user can then get the amount of hours, minutes
/// and seconds from this object by calling the needed function; Respectively getHours(), getMinutes() and getSeconds().
timeData DS3231::getTime(){
	getStatus();
	return time;
}

/// \brief
/// Get Date
/// \details
/// This function returns the current date as an object of type dateData. The user can then get the day of week, day of month
/// , month and year from this object by calling the needed function; Respectively getWeekDay(), getMonthDay(), getMonth() and getYear().
dateData DS3231::getDate(){
	getStatus();
	return date;
}

/// \brief
/// Set First Alarm
/// \details
/// This function enables the alarm at the time set by calling the function changeFirstAlarm(). The matchconditions determine when
/// the alarm should be triggered. The available conditions are:
///		- 15 (alarm once per second)
///		- 13 (alarm when seconds match; so at least once per minute)
///		- 12 (alarm when minutes and seconds match; so at least once an hour)
/// 	- 8 (alarm when hours, minutes and seconds match)
///		- 0 (alarm when weekDay, hours, minutes and seconds match)
///		- 16 (alarm when monthDay, hours, minutes and seconds match. 16 to enable easy and fast bit operation)
/// When the outputSignal boolean is true (which it doesn't default to) the SQW output becomes high when the signal is triggered.
/// Depending on the match conditions, the alarm should be checked every second, minute, hour or day.
void DS3231::setFirstAlarm(const unsigned int matchConditions, const bool dateCondition, const bool outputSignal){
	firstAlarm.enableOutputSignal(outputSignal);
	firstAlarm.setMatchConditions(matchConditions);
	auto transaction = bus.write(address);
	transaction.write(0x07);		//0x07 for ALARM1, 0x11 for ALARM2
	transaction.write((((firstAlarm.time.getSeconds() / 10) & 0x07) << 4) + ((firstAlarm.time.getSeconds() % 10) & 0x0F) + ((firstAlarm.getMatchConditions() & 0x01) << 7));
	transaction.write((((firstAlarm.time.getMinutes() / 10) & 0x07) << 4) + ((firstAlarm.time.getMinutes() % 10) & 0x0F) + ((firstAlarm.getMatchConditions() & 0x02) << 6));
	transaction.write((((firstAlarm.time.getHours() / 10) & 0x01) << 4) + ((firstAlarm.time.getHours() % 10) & 0x0F) + ((firstAlarm.getMatchConditions() & 0x04) << 5));
	if(dateCondition){
		//If the Day of Week has to match, 1 has to be written to 6th bit and weekDay has to be written as well.
		transaction.write((firstAlarm.date.getWeekDay() & 0x0F) + ((firstAlarm.getMatchConditions() & 0x08) << 4) + (((dateCondition) & 1) << 6));
	} else {
		transaction.write((((firstAlarm.date.getMonthDay() / 10) & 0x03) << 4) + ((firstAlarm.date.getMonthDay() % 10) & 0x0F) + ((firstAlarm.getMatchConditions() & 0x08) << 4) + (((dateCondition) & 1) << 6));
	}
}

/// \brief
/// Change First Alarm Time
/// \details
/// This function changes the time at which the alarm is set. When the given time and date are reached,
/// the output (SKW) becomes high if outputSignal with setFirstAlarm() is set. For the alarm to trigger the updateAlarms()
/// function has got to be called.
void DS3231::changeFirstAlarm(const timeData & alarmTime, const dateData & alarmDate){
	//Change private attibutes
	firstAlarm.time = alarmTime;
	firstAlarm.date = alarmDate;
}

/// \brief
/// Set Second Alarm
/// \details
/// This function enables the alarm at the time set by calling the function changeSecondAlarm(). The matchconditions determine when
/// the alarm should be triggered. The available conditions are:
///		- 7 (alarm once per minute)
///		- 6 (alarm when minutes match)
/// 	- 8 (alarm when hours and minutes)
///		- 0 (alarm when weekDay, hours and minutes match)
///		- 16 (alarm when monthDay, hours and minutes match)
/// When the outputSignal boolean is true (which it doesn't default to) the SQW output becomes high when the signal is triggered.
/// Depending on the match conditions, the alarm should be checked every second, minute, hour or day.
void DS3231::setSecondAlarm(const unsigned int matchConditions, const bool dateCondition, const bool outputSignal){
	secondAlarm.enableOutputSignal(outputSignal);
	secondAlarm.setMatchConditions(matchConditions);
	auto transaction = bus.write(address);
	transaction.write(0x11);		//0x07 for ALARM1, 0x11 for ALARM2
	transaction.write((((secondAlarm.time.getSeconds() / 10) & 0x07) << 4) + ((secondAlarm.time.getSeconds() % 10) & 0x0F) + ((secondAlarm.getMatchConditions() & 0x01) << 7));
	transaction.write((((secondAlarm.time.getMinutes() / 10) & 0x07) << 4) + ((secondAlarm.time.getMinutes() % 10) & 0x0F) + ((secondAlarm.getMatchConditions() & 0x02) << 6));
	transaction.write((((secondAlarm.time.getHours() / 10) & 0x01) << 4) + ((secondAlarm.time.getHours() % 10) & 0x0F)  + ((secondAlarm.getMatchConditions() & 0x03) << 5));
	if(dateCondition){
		transaction.write((secondAlarm.date.getWeekDay() & 0x0F)  + ((secondAlarm.getMatchConditions() & 0x04) << 7) + (((dateCondition) & 1) << 6));
	} else {
		transaction.write((((secondAlarm.date.getMonthDay() / 10) & 0x03) << 4) + ((secondAlarm.date.getMonthDay() % 10) & 0x0F) + ((secondAlarm.getMatchConditions() & 0x04) << 7) + (((dateCondition) & 1) << 6));
	}
}

/// \brief
/// Change Second Alarm Time
/// \details
/// This function changes the time at which the alarm is set. When the given time and date are reached,
/// the output (SKW) becomes high if outputSignal with setSecondAlarm() is set. For the alarm to trigger the updateAlarms()
/// function has got to be called.
void DS3231::changeSecondAlarm(const timeData & alarmTime, const dateData & alarmDate){
	//Change private attibutes
	secondAlarm.time = alarmTime;
	secondAlarm.date = alarmDate;
}

/// \brief
/// Update Alarm State
/// \details
/// This function retrieves the state of the alarms and puts the state in specific the attributes.
void DS3231::updateAlarms(){
	bus.write(address).write(0x0F);
	auto transaction = bus.read(address);
	auto states = transaction.read_byte();
	firstAlarmState = (states & 1UL);
	secondAlarmState = (states >> 1) & 1UL;
}

/// \brief
/// Check Alarm State
/// \details
/// This function returns the current state of the alarms. It can output 3 (if both alarms are triggered), 
/// 2 (if alarm 2 is triggered), 1 (if alarm 1 is triggered) or 0 (if none alarm is triggered). Calls updateAlarms()
/// in the background to execute based on recent values.
unsigned int DS3231::checkAlarms(){
	updateAlarms();
	if(firstAlarmState && secondAlarmState){
		firstAlarmState = false;
		secondAlarmState = false;
		return 3;
	} else if (firstAlarmState){
		firstAlarmState = false;
		secondAlarmState = false;
		return 1;
	} else if (secondAlarmState){
		firstAlarmState = false;
		secondAlarmState = false;
		return 2;
	} else {
		firstAlarmState = false;
		secondAlarmState = false;
		return 0;
	}
}

/// \brief
/// Get Temperature
/// \details
/// This function returns the current temperature of the surrounding area of the chip. Can reach accuracies of up to 
/// 0.25 Degrees Celcius.
unsigned int DS3231::getTemperature(){
	bus.write(address).write(0x11);
	auto transaction = bus.read(address);
	unsigned int temperature = transaction.read_byte();
	temperature = (temperature << 2);
	temperature |= (transaction.read_byte() >> 6);
	return temperature * 0.25;
}
