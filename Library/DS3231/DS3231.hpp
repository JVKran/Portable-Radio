#ifndef __DS3231_HPP
#define __DS3231_HPP

class timeData{
	private:
		unsigned int hours;
		unsigned int minutes;
		unsigned int seconds;
		bool PM = false;
	public:
		timeData(const unsigned int hours = 0, const unsigned int minutes = 0, const unsigned int seconds = 0);

		void setHours(const unsigned int recHours);
		void setMinutes(const unsigned int recMinutes);
		void setSeconds(const unsigned int recSeconds);

		unsigned int getHours();
		unsigned int getMinutes();
		unsigned int getSeconds();
};

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
};

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