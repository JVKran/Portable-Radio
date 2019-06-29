/// @file

#include "hwlib.hpp"
#include "DS3231.hpp"

/// \brief
/// Constructor
/// \details
/// This constructor has no mandatory parameters. Though the user can provide the hours, minutes and seconds to initialize
/// the ADT with those values.
timeData::timeData(const unsigned int hours, const unsigned int minutes, const unsigned int seconds):
	hours(hours),
	minutes(minutes),
	seconds(seconds)
{}

/// \brief
/// Set Hours
/// \details
/// This function has one mandatory parameter; the current amount of hours. This function sets the hours to the
/// given amount.
void timeData::setHours(const unsigned int recHours){
	hours = recHours;
}

/// \brief
/// Set Minutes
/// \details
/// This function has one mandatory parameter; the current amount of minutes. This function sets the minutes to the
/// given amount.
void timeData::setMinutes(const unsigned int recMinutes){
	minutes = recMinutes;
}

/// \brief
/// Set Seconds
/// \details
/// This function has one mandatory parameter; the current amount of seconds. This function sets the seconds to the
/// given amount.
void timeData::setSeconds(const unsigned int recSeconds){
	seconds = recSeconds;
}

/// \brief
/// Get Hours
/// \details
/// This function returns the current set hours.
unsigned int timeData::getHours(){
	return hours;
}

/// \brief
/// Get Minutes
/// \details
/// This function returns the current set minutes.
unsigned int timeData::getMinutes(){
	return minutes;
}

/// \brief
/// Get Seconds
/// \details
/// This function returns the current set seconds.
unsigned int timeData::getSeconds(){
	return seconds;
}

//<<<------------------------------------------------------------------------------------------>>>

/// \brief
/// Constructor
/// \details
/// This constructor has no mandatory parameters. Though the user can provide the Day of Week, Day of Month, Month and year to
/// initialize the ADT with those values.
dateData::dateData(const unsigned int weekDay, const unsigned int monthDay, const unsigned int month, const unsigned int year):
	weekDay(weekDay),
	monthDay(monthDay),
	month(month),
	year(year)
{}

/// \brief
/// Set Day of Week
/// \details
/// This constructor has one mandatory parameter; the day of week. This function sets the Day of Week
/// to the given amount.
void dateData::setWeekDay(const unsigned int recWeekDay){
	weekDay = recWeekDay;
}

/// \brief
/// Set Day of Month
/// \details
/// This constructor has one mandatory parameter; the day of Month. This function sets the Day of Month
/// to the given amount.
void dateData::setMonthDay(const unsigned int recMonthDay){
	monthDay = recMonthDay;
}

/// \brief
/// Set Month
/// \details
/// This constructor has one mandatory parameter; the Month. This function sets the Month
/// to the given amount.
void dateData::setMonth(const unsigned int recMonth){
	month = recMonth;
}

/// \brief
/// Set Year
/// \details
/// This constructor has one mandatory parameter; the year. This function sets the year
/// to the given amount.
void dateData::setYear(const unsigned int recYear){
	year = recYear;
}

/// \brief
/// Get Day of Week
/// \details
/// This function returns the currently set Day of Week;
unsigned int dateData::getWeekDay(){
	return weekDay;
}

/// \brief
/// Get Day of Month
/// \details
/// This function returns the currently set Day of Month;
unsigned int dateData::getMonthDay(){
	return monthDay;
}

/// \brief
/// Get Month
/// \details
/// This function returns the currently set Month;
unsigned int dateData::getMonth(){
	return month;
}

/// \brief
/// Get Year
/// \details
/// This function returns the currently set Year;
unsigned int dateData::getYear(){
	return year;
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
/// This function enables the alarm at the time set by calling the function changeFirstAlarm().
void DS3231::setFirstAlarm(const unsigned int matchConditions, const bool outputSignal){
	firstAlarm.enableOutputSignal(outputSignal);
	firstAlarm.setMatchConditions(matchConditions);
	auto transaction = bus.write(address);
	transaction.write(0x07);		//0x07 for ALARM1, 0x11 for ALARM2
	transaction.write((((firstAlarm.time.getSeconds() / 10) & 0x07) << 4) + ((firstAlarm.time.getSeconds() % 10) & 0x0F));
	transaction.write((((firstAlarm.time.getMinutes() / 10) & 0x07) << 4) + ((firstAlarm.time.getMinutes() % 10) & 0x0F));
	transaction.write((((firstAlarm.time.getHours() / 10) & 0x01) << 4) + ((firstAlarm.time.getHours() % 10) & 0x0F));
	transaction.write(firstAlarm.date.getWeekDay() & 0x0F);
	transaction.write((((firstAlarm.date.getMonthDay() / 10) & 0x03) << 4) + ((firstAlarm.date.getMonthDay() % 10) & 0x0F));
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
/// Set SecondAlarm
/// \details
/// This function enables the alarm at the time set by calling the function changeSecondAlarm().
void DS3231::setSecondAlarm(const unsigned int matchConditions, const bool outputSignal){
	secondAlarm.enableOutputSignal(outputSignal);
	secondAlarm.setMatchConditions(matchConditions);
	auto transaction = bus.write(address);
	transaction.write(0x07);		//0x07 for ALARM1, 0x11 for ALARM2
	transaction.write((((secondAlarm.time.getSeconds() / 10) & 0x07) << 4) + ((secondAlarm.time.getSeconds() % 10) & 0x0F));
	transaction.write((((secondAlarm.time.getMinutes() / 10) & 0x07) << 4) + ((secondAlarm.time.getMinutes() % 10) & 0x0F));
	transaction.write((((secondAlarm.time.getHours() / 10) & 0x01) << 4) + ((secondAlarm.time.getHours() % 10) & 0x0F));
	transaction.write(secondAlarm.date.getWeekDay() & 0x0F);
	transaction.write((((secondAlarm.date.getMonthDay() / 10) & 0x03) << 4) + ((secondAlarm.date.getMonthDay() % 10) & 0x0F));
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