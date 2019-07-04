/// @file

#include "hwlib.hpp"
#include "DS3231.hpp"

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
/// Constructor
/// \details
/// This constructor has one mandatory parameter; an object of type alarm. It copies the given values.
alarm::alarm(const alarm & givenAlarm){
	matchConditions = givenAlarm.matchConditions;
	outputSignal = givenAlarm.outputSignal;
	alarmNumber = givenAlarm.alarmNumber;
}

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
/// This constructor has one mandatory parameter; the I2C bus. The user can also provide the address and resetPin.
/// The address defaults to 0x68 and the resetPin defaults to a hwlib::pin_in_out_dummy (which always returns 0).
DS3231::DS3231(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address, hwlib::pin_in_out & resetPin):
	bus(bus),
	address(address),
	resetPin(resetPin),
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
/// Set Time
/// \details
/// This function has one mandatory parameter; a timeData object to set the time to.
void DS3231::setTime(const timeData & time){
	setTime(time.getHours(), time.getMinutes(), time.getSeconds());
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
/// Set Date
/// \details
/// This function has only one mandatory parameter; a dateData object which to set the date to.
void DS3231::setDate(const dateData & date){
	setDate(date.getWeekDay(), date.getMonthDay(), date.getMonth(), date.getYear());
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
///		- 14 (alarm when seconds match; so at least once per minute)
///		- 12 (alarm when minutes and seconds match; so at least once an hour)
/// 	- 8 (alarm when hours, minutes and seconds match)
///		- 0 (alarm when weekDay, hours, minutes and seconds match)
///		- 16 (alarm when monthDay, hours, minutes and seconds match. 16 to enable easy and fast bit operation)
/// When the outputSignal boolean is true (which it doesn't default to) the SQW output becomes high when the signal is triggered. The alarm-trigger-bit remains high
/// though; it can be checked later. Unless the bit is cleared with clearAlarm(). For the made changes to have effect, setSecondAlarm() has also
/// got to be called.
void DS3231::setFirstAlarm(const unsigned int matchConditions, const bool dateCondition, const bool outputSignal){
	clearAlarm(1);
	firstAlarm.enableOutputSignal(outputSignal);
	firstAlarm.setMatchConditions(matchConditions);
	auto transaction = bus.write(address);
	transaction.write(0x07);		//0x07 for ALARM1, 0x11 for ALARM2
	transaction.write((((firstAlarm.time.getSeconds() / 10) & 0x07) << 4) + ((firstAlarm.time.getSeconds() % 10) & 0x0F) + ((secondAlarm.getMatchConditions() & 0x01) << 7));
	transaction.write((((firstAlarm.time.getMinutes() / 10) & 0x07) << 4) + ((firstAlarm.time.getMinutes() % 10) & 0x0F) + ((secondAlarm.getMatchConditions() & 0x02) << 6));
	transaction.write((((firstAlarm.time.getHours() / 10) & 0x01) << 4) + ((firstAlarm.time.getHours() % 10) & 0x0F) + ((secondAlarm.getMatchConditions() & 0x03) << 5));
	if(dateCondition){
		//If the Day of Week has to match, 1 has to be written to 6th bit and weekDay has to be written as well.
		transaction.write((firstAlarm.date.getWeekDay() & 0x0F) + ((firstAlarm.getMatchConditions() & 0x04) << 4) + (((dateCondition) & 1) << 6));
	} else {
		transaction.write((((firstAlarm.date.getMonthDay() / 10) & 0x03) << 4) + ((firstAlarm.date.getMonthDay() % 10) & 0x0F) + ((secondAlarm.getMatchConditions() & 0x04) << 4) + (((dateCondition) & 1) << 6));
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
/// Depending on the match conditions, the alarm should be checked every second, minute, hour or day. The alarm-trigger-bit remains high
/// though; it can be checked later. Unless the bit is cleared with clearAlarm(). For the made changes to have effect, setFirstAlarm() has also
/// got to be called.
void DS3231::setSecondAlarm(const unsigned int matchConditions, const bool dateCondition, const bool outputSignal){
	clearAlarm(2);
	secondAlarm.enableOutputSignal(outputSignal);
	secondAlarm.setMatchConditions(matchConditions);
	auto transaction = bus.write(address);
	transaction.write(0x11);		//0x07 for ALARM1, 0x11 for ALARM2
	transaction.write((((secondAlarm.time.getSeconds() / 10) & 0x07) << 4) + ((secondAlarm.time.getSeconds() % 10) & 0x0F) + ((secondAlarm.getMatchConditions() & 0x01) << 7));
	transaction.write((((secondAlarm.time.getMinutes() / 10) & 0x07) << 4) + ((secondAlarm.time.getMinutes() % 10) & 0x0F) + ((secondAlarm.getMatchConditions() & 0x02) << 6));
	transaction.write((((secondAlarm.time.getHours() / 10) & 0x01) << 4) + ((secondAlarm.time.getHours() % 10) & 0x0F)  + ((secondAlarm.getMatchConditions() & 0x03) << 5));
	if(dateCondition){
		transaction.write((secondAlarm.date.getWeekDay() & 0x0F)  + ((secondAlarm.getMatchConditions() & 0x04) << 4) + (((dateCondition) & 1) << 6));
	} else {
		transaction.write((((secondAlarm.date.getMonthDay() / 10) & 0x03) << 4) + ((secondAlarm.date.getMonthDay() % 10) & 0x0F) + ((secondAlarm.getMatchConditions() & 0x04) << 4) + (((dateCondition) & 1) << 6));
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
/// This function retrieves the state of the alarms and puts the state in the representative attributes.
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
/// When the alarm is triggered, the alarm bit representing the state of the alarm remains triggered. Thus, the alarm does not have to 
/// be checked after a specific amount of time.
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
/// Clear / Unset Alarms
/// \details
/// This function clears / unsets / disarms the alarm whose alarmnumber is equal to the passed argument; it sets / resets the alarm flag
/// to 0. Howver, there is no such way to tell the chip the alarm should be off. There always is a state in which the alarm will trigger.
/// Depending on the Match Conditions, the alarm will keep triggering every second, minute, hour, day, month or year. Thus, the result of
/// checkAlarms() should only be taken seriously after the alarm has been set.
void DS3231::clearAlarm(const unsigned int alarmNumber){
	bus.write(address).write(0x0F);
	bus.read(address).read(status, 1);
	if(alarmNumber == 1){
		status[0] &= ~1UL;
	} else {
		status[0] &= ~2UL;
	}
	auto transaction = bus.write(address);
	transaction.write(0x0F);
	transaction.write(status[0]);
}

/// \brief
/// Get Temperature
/// \details
/// This function returns the current temperature of the surrounding area of the chip. Can reach accuracies of up to 
/// 0.25 Degrees Celcius.
float DS3231::getTemperature(){
	bus.write(address).write(0x11);
	auto transaction = bus.read(address);
	unsigned int temperature = transaction.read_byte();
	temperature = (temperature << 2);
	temperature |= (transaction.read_byte() >> 6);
	return temperature * 0.25;
}

/// \brief
/// Set/Unset Reset
/// \details
/// This function makes the reset-pin high or low depending on the passed argument; defaults to true (reset).
void DS3231::setReset(const bool reset){
	resetPin.direction_set_output();
	resetPin.write(reset);
	resetPin.flush();
}

/// \brief
/// Get Reset State
/// \details
/// This function returns true if reset pin has been made high; false if low.
bool DS3231::getReset(){
	resetPin.direction_set_input();
	resetPin.refresh();
	return resetPin.read();
}

/// \brief
/// Test Correct Functioning
/// \details
/// This function performs a quick test to figure out if the chip is working.
bool DS3231::testCorrectFunctioning(){
	auto storedTime = getTime();
	auto checkTime = timeData(11, 11, 11);
	setTime(checkTime);
	if(getTime() == checkTime){
		setTime(storedTime);
		return true;
	} else {
		setTime(storedTime);
		return false;
	}
}
