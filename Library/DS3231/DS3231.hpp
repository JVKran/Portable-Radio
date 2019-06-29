#ifndef __DS3231_HPP
#define __DS3231_HPP

class timeData{
	private:
		unsigned int hours;
		unsigned int minutes;
		unsigned int seconds;
	public:
		timeData(const unsigned int hours = 0, const unsigned int minutes = 0, const unsigned int seconds = 0);
		void setHours(const unsigned int recHours);
		void setMinutes(const unsigned int recMinutes);
		void setSeconds(const unsigned int recSeconds);
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

};

class DS3231{
	private:
		hwlib::i2c_bus & bus;
		const uint8_t address;
		uint8_t data[13] = {};
		uint8_t status[8] = {};
		void setData();
	public:
		DS3231(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address = 0x68);
		timeData time;
		dateData date;
		void getStatus();
};

#endif //__DS3231_HPP