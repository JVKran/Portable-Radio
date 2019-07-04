/// @file

#ifndef __DS3231_HPP
#define __DS3231_HPP

/// \brief
/// Time ADT
/// \details
/// This is an ADT that is able to store hours, minutes and seconds. It is also possible to do simple operations
/// and change or get the stored values.
///	All supported operations are:
///		- Get Hours, Minutes or seconds
///		- Set Hours, Minutes or seconds
///		- Arithmetics: +, +=, -, -=, ==, !=, <, <=, > and >=
///
/// ~~~~~~~~~~~~~~~{.cpp}
/// auto clock = DS3231(i2c_bus);
/// auto curTime = clock.getTime();
/// timeData time;
/// dateData date;
/// 
/// for(;;){
///     time = clock.getTime();
///     date = clock.getDate();
///     
///     hwlib::cout << "Time: " << time.getHours() << ":" << time.getMinutes() << ":" << time.getSeconds() << hwlib::endl;
///     hwlib::cout << "Temperature: " << clock.getTemperature() << hwlib::endl;
///     hwlib::cout << "Date: " << date.getMonthDay() << "-" << date.getMonth() << "-" << date.getYear() << hwlib::endl << hwlib::endl;
///     curTime = clock.getTime();
///     curTime.setSeconds(curTime.getSeconds() + 10);
///   	clock.changeFirstAlarm(curTime, dateData(0, 0, 1, 2019));
///   	clock.setFirstAlarm(14);
///  	hwlib::cout << "Alarm set, should go in 10 seconds: ";
/// 
///   	hwlib::wait_ms(30);
/// 
///   	while(clock.checkAlarms() == 0){
///     	hwlib::wait_ms(200);
///	  	}
///		
///		hwlib::cout << curTime - clock.getTime() << "seconds have passed" << hwlib::endl;
/// 
///   	hwlib::cout << "Triggered!" << hwlib::endl;
///	}
/// ~~~~~~~~~~~~~~~
class timeData{
	private:
		unsigned int hours;
		unsigned int minutes;
		unsigned int seconds;
	public:
		timeData(const unsigned int givenHours = 0, const unsigned int givenMinutes = 0, const unsigned int givenSeconds = 0);
		timeData(const timeData & time);

		//Setters because we don't want times of 99:99:99 possible; the max should be 23:59:95
		void setHours(const unsigned int rgivenHours);
		void setMinutes(const unsigned int givenMinutes);
		void setSeconds(const unsigned int givenSeconds);

		void setTime(const timeData & time);
		void setTime(const unsigned int givenHours = 0, const unsigned int givenMinutes = 0, const unsigned int givenSeconds = 0);

		unsigned int getHours() const;
		unsigned int getMinutes() const;
		unsigned int getSeconds() const;

		timeData operator+(const timeData & rhs) const;
		timeData & operator+=(const timeData & rhs);

		timeData operator-(const timeData & rhs) const;
		timeData & operator-=(const timeData & rhs);

		bool operator==(const timeData & rhs) const;
		bool operator!=(const timeData & rhs) const;

		bool operator>(const timeData & rhs) const;
		bool operator>=(const timeData & rhs) const;

		bool operator<(const timeData & rhs) const;
		bool operator<=(const timeData & rhs) const;

		/// \brief
		/// Operator<<
		/// \details
		/// This operator makes it possible to print the time in format "hour:minutes:seconds".
		friend hwlib::ostream & operator<<(hwlib::ostream & lhs, const timeData & rhs){
		  lhs << rhs.getHours() << ":" << rhs.getMinutes() << ":" << rhs.getSeconds();
		  return lhs;
		}
};

/// \brief
/// Date ADT
/// \details
/// This is an ADT that is able to store the Day of Week, Daynumber, Month and Year. It is also possible to do simple operations
/// and change or get the stored values.
///	All supported operations are:
///		- Get Day of Week, Day of Month, Month and Year
///		- Set Day of Week, Day of Month, Month and Year
///		- Arithmetics: +, +=, -, -=, == and !=
///
/// ~~~~~~~~~~~~~~~{.cpp}
/// auto clock = DS3231(i2c_bus);
/// auto curTime = clock.getTime();
/// timeData time;
/// dateData date;
/// 
/// for(;;){
///     time = clock.getTime();
///     date = clock.getDate();
///     
///     hwlib::cout << "Time: " << time.getHours() << ":" << time.getMinutes() << ":" << time.getSeconds() << hwlib::endl;
///     hwlib::cout << "Temperature: " << clock.getTemperature() << hwlib::endl;
///     hwlib::cout << "Date: " << date.getMonthDay() << "-" << date.getMonth() << "-" << date.getYear() << hwlib::endl << hwlib::endl;
///     curTime = clock.getTime();
///     curTime.setSeconds(curTime.getSeconds() + 10);
///   	clock.changeFirstAlarm(curTime, dateData(0, 0, 1, 2019));
///   	clock.setFirstAlarm(14);
///  	hwlib::cout << "Alarm set, should go in 10 seconds: ";
/// 
///   	hwlib::wait_ms(30);
/// 
///   	while(clock.checkAlarms() == 0){
///     	hwlib::wait_ms(200);
///	  	}
///		
///		hwlib::cout << curTime - clock.getTime() << "seconds have passed" << hwlib::endl;
/// 
///   	hwlib::cout << "Triggered!" << hwlib::endl;
///	}
/// ~~~~~~~~~~~~~~~
class dateData{
	private:
		unsigned int weekDay;
		unsigned int monthDay;
		unsigned int month;
		unsigned int year;
	public:
		dateData(const unsigned int givenWeekDay = 0, const unsigned int givenMonthDay = 0, const unsigned int givenMonth = 0, const unsigned int givenYear = 0);
		dateData(const dateData & date);

		//Setters because we don't want dates of 99-99-9999; the max should be 30-12-9999
		void setWeekDay(const unsigned int givenWeekDay);
		void setMonthDay(const unsigned int givenMonthDay);
		void setMonth(const unsigned int givenMonth);
		void setYear(const unsigned int givenYear);

		void setDate(const dateData & date);
		void setDate(const unsigned int givenWeekDay = 0, const unsigned int givenMonthDay = 0, const unsigned int givenMonth = 0, const unsigned int givenYear = 0);

		unsigned int getWeekDay() const;
		unsigned int getMonthDay() const;
		unsigned int getMonth() const;
		unsigned int getYear() const;

		dateData operator+(const dateData & rhs) const;
		dateData & operator+=(const dateData & rhs);

		dateData operator-(const dateData & rhs) const;
		dateData & operator-=(const dateData & rhs);

		bool operator==(const dateData & rhs) const;
		bool operator!=(const dateData & rhs) const;

		bool operator<(const dateData & rhs) const;
		bool operator<=(const dateData & rhs) const;

		bool operator>(const dateData & rhs) const;
		bool operator>=(const dateData & rhs) const;

		/// \brief
		/// Operator<<
		/// \details
		/// This operator makes it possible to print the date in format "monthDay-Month-Year".
		friend hwlib::ostream & operator<<(hwlib::ostream & lhs, const dateData & rhs){
		  lhs << rhs.getMonthDay() << "-" << rhs.getMonth() << "-" << rhs.getYear();
		  return lhs;
		}
};

/// \brief
/// Alarm Class
/// \details
/// This is a class that keeps track of the set alarm; managing when it should be triggered and how it should be triggered.
/// It is also possible to determine if the output (SKW-pin) should be set high if the alarm triggers. The matchconditions default to 16
/// (monthDay, hours, minutes and seconds match), which is the same for both alarms.
///	All supported operations are:
///		- Get Match Condition
///		- Set Match Condition or Output Signal
///		- Arithmetics: +, +=, -, -=, == and !=
///
/// ~~~~~~~~~~~~~~~{.cpp}
/// auto clock = DS3231(i2c_bus);
/// auto curTime = clock.getTime();
/// timeData time;
/// dateData date;
/// 
/// for(;;){
///     time = clock.getTime();
///     date = clock.getDate();
///     
///     hwlib::cout << "Time: " << time.getHours() << ":" << time.getMinutes() << ":" << time.getSeconds() << hwlib::endl;
///     hwlib::cout << "Temperature: " << clock.getTemperature() << hwlib::endl;
///     hwlib::cout << "Date: " << date.getMonthDay() << "-" << date.getMonth() << "-" << date.getYear() << hwlib::endl << hwlib::endl;
///     curTime = clock.getTime();
///     curTime.setSeconds(curTime.getSeconds() + 10);
///   	clock.changeFirstAlarm(curTime, dateData(0, 0, 1, 2019));
///   	clock.setFirstAlarm(14);
///  	hwlib::cout << "Alarm set with matchconditions (" << firstAlarm.getMatchConditions() << ", should go in 10 seconds: ";
/// 
///   	hwlib::wait_ms(30);
/// 
///   	while(clock.checkAlarms() == 0){
///     	hwlib::wait_ms(200);
///	  	}
///		
///		hwlib::cout << curTime - clock.getTime() << "seconds have passed" << hwlib::endl;
/// 
///   	hwlib::cout << "Triggered!" << hwlib::endl;
///	}
/// ~~~~~~~~~~~~~~~
class alarm{
	private:
		unsigned int matchConditions;
		bool outputSignal;
		unsigned int alarmNumber;
	public:
		alarm(const unsigned int matchConditions = 16, const bool outputSignal = 0);
		alarm(const alarm & givenAlarm);

		void setMatchConditions(const unsigned int condition = 0);
		unsigned int getMatchConditions();

		void enableOutputSignal(const bool enable);

		timeData time;
		dateData date;
};

/// \brief
/// DS3231 Interface
/// \details
/// This is an Interface for the DS3231 RTC (Realtime-Clock) that makes it easier to retrieve the current time and date. It also
/// simplifies the setting of the current time and date since it is also handling the change of century. It is also possible to
/// get the temperature from the onboard thermometer which can supply an accuracy of up to 0.25 Degrees Celcius.
/// 
///	All supported operations are:
///		- Get Time
///		- Get Date
///		- Set Alarm
///		- Unset Alarm
/// 	- Get Temperature
///
/// ~~~~~~~~~~~~~~~{.cpp}
/// auto clock = DS3231(i2c_bus);
///	clock.setTime(23, 59, 0);
/// clock.setDate(1, 31, 12, 1999);	//Century change is taken care of
///	
/// auto curTime = clock.getTime();
/// timeData time;
/// dateData date;
/// 
/// for(;;){
///     time = clock.getTime();
///     date = clock.getDate();
///     
///     hwlib::cout << "Time: " << time.getHours() << ":" << time.getMinutes() << ":" << time.getSeconds() << hwlib::endl;
///     hwlib::cout << "Temperature: " << clock.getTemperature() << hwlib::endl;
///     hwlib::cout << "Date: " << date.getMonthDay() << "-" << date.getMonth() << "-" << date.getYear() << hwlib::endl << hwlib::endl;
///	
///     curTime = clock.getTime();
///     curTime.setSeconds(curTime.getSeconds() + 10);
///   	clock.changeFirstAlarm(curTime, dateData(0, 0, 1, 2019));
///   	clock.setFirstAlarm(14);
///  	hwlib::cout << "Alarm set, should go in 10 seconds: ";
/// 
///   	hwlib::wait_ms(30);
/// 
///   	while(clock.checkAlarms() == 0){
///     	hwlib::wait_ms(200);
///	  	}
/// 
///   	hwlib::cout << "Triggered!" << hwlib::endl;
///	}
/// ~~~~~~~~~~~~~~~
class DS3231{
	private:
		hwlib::i2c_bus & bus;
		const uint8_t address;
		hwlib::pin_in_out & resetPin;
		uint8_t data[13] = {};
		uint8_t status[13] = {};
		void setData();
		void getStatus();

		bool firstAlarmState = false;
		bool secondAlarmState = false;
		alarm firstAlarm;
		alarm secondAlarm;

		timeData time;
		dateData date;
	public:
		DS3231(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x68, hwlib::pin_in_out & resetPin = hwlib::pin_in_out_dummy);

		void setTime(const unsigned int hours, const unsigned int minutes, const unsigned int seconds = 0);
		void setTime(const timeData & time);

		void setDate(const unsigned int weekDay, const unsigned int monthDay, const unsigned int month, const unsigned int year);
		void setDate(const dateData & date);

		timeData getTime();
		dateData getDate();

		void changeFirstAlarm(const timeData & alarmTime, const dateData & alarmDate);
		void setFirstAlarm(const unsigned int matchConditions, const bool dateCondition = true, const bool outputSignal = false);

		void changeSecondAlarm(const timeData & alarmTime, const dateData & alarmDate);
		void setSecondAlarm(const unsigned int matchConditions, const bool dateCondition = true, const bool outputSignal = false);

		void updateAlarms();
		unsigned int checkAlarms();
		void clearAlarm(const unsigned int alarmNumber);

		void setReset(const bool reset = true);
		bool getReset();

		unsigned int getAddress();

		float getTemperature();
};

#endif //__DS3231_HPP
