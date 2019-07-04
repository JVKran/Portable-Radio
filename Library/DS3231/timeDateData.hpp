/// @file

#ifndef __TIMEDATE_DATA_HPP
#define __TIMEDATE_DATA_HPP

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
///
///		clock.clearAlarm(1);
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

#endif //__TIMEDATE_DATA_HPP