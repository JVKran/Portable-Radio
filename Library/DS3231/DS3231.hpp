/// @file

#ifndef __DS3231_HPP
#define __DS3231_HPP

/// \brief
/// Time ADT
/// \details
/// This is an ADT that is able to store hours, minutes and seconds. It is also possible to do simple operations
/// and change or get the stored values.
class timeData{
	private:
		unsigned int hours;
		unsigned int minutes;
		unsigned int seconds;
	public:
		timeData(const unsigned int givenHours = 0, const unsigned int givenMinutes = 0, const unsigned int givenSeconds = 0);

		void setHours(const unsigned int rgivenHours);
		void setMinutes(const unsigned int givenMinutes);
		void setSeconds(const unsigned int givenSeconds);
		void setTime(const unsigned int givenHours = 0, const unsigned int givenMinutes = 0, const unsigned int givenSeconds = 0);

		unsigned int getHours() const;
		unsigned int getMinutes() const;
		unsigned int getSeconds() const;

		timeData operator+(const timeData & rhs) const;
		timeData & operator+=(const timeData & rhs);

		timeData operator-(const timeData & rhs) const;
		timeData & operator-=(const timeData & rhs);

		timeData operator*(const timeData & rhs) const;
		timeData & operator*=(const timeData & rhs);

		bool operator==(const timeData & rhs) const;
};

/// \brief
/// Date ADT
/// \details
/// This is an ADT that is able to store the Day of Week, Daynumber, Month and Year. It is also possible to do simple operations
/// and change or get the stored values.
class dateData{
	private:
		unsigned int weekDay;
		unsigned int monthDay;
		unsigned int month;
		unsigned int year;
	public:
		dateData(const unsigned int weekDay = 0, const unsigned int monthDay = 0, const unsigned int month = 0, const unsigned int year = 0);

		void setWeekDay(const unsigned int recWeekDay);
		void setMonthDay(const unsigned int recMonthDay);
		void setMonth(const unsigned int recMonth);
		void setYear(const unsigned int recYear);

		unsigned int getWeekDay();
		unsigned int getMonthDay();
		unsigned int getMonth();
		unsigned int getYear();

		/*
		dateData operator+(const dateData & rhs) const;
		dateData & operator+=(const dateData & rhs);

		dateData operator-(const dateData & rhs) const;
		dateData & operator-=(const dateData & rhs);

		dateData operator*(const dateData & rhs) const;
		dateData & operator*=(const dateData & rhs);
		*/
};

/// \brief
/// Alarm Class
/// \details
/// This is a class that keeps track of the set alarm; managing when it should be triggered and how it should be triggered.
/// It is also possible to determine if the output (SKW-pin) should be set high if the alarm triggers.
class alarm{
	private:
		unsigned int matchConditions;
		bool outputSignal;
		unsigned int alarmNumber;
	public:
		alarm(const unsigned int matchConditions = 0, const bool outputSignal = 0);
		void setMatchConditions(const unsigned int condition = 0);
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
class DS3231{
	private:
		hwlib::i2c_bus & bus;
		const uint8_t address;
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
		DS3231(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x68);

		void setTime(const unsigned int hours, const unsigned int minutes, const unsigned int seconds = 0);
		void setDate(const unsigned int weekDay, const unsigned int monthDay, const unsigned int month, const unsigned int year);

		timeData getTime();
		dateData getDate();

		void changeFirstAlarm(const timeData & alarmTime, const dateData & alarmDate);
		void setFirstAlarm(const unsigned int matchConditions, const bool outputSignal = false);

		void changeSecondAlarm(const timeData & alarmTime, const dateData & alarmDate);
		void setSecondAlarm(const unsigned int matchConditions, const bool outputSignal = false);

		void updateAlarms();
		unsigned int checkAlarms();

		unsigned int getTemperature();
};

#endif //__DS3231_HPP