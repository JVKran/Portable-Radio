#include "hwlib.hpp"
#include "DS3231.hpp"

timeData::timeData(const unsigned int hours, const unsigned int minutes, const unsigned int seconds):
	hours(hours),
	minutes(minutes),
	seconds(seconds)
{}

void timeData::setHours(const unsigned int recHours){
	hours = recHours;
}

void timeData::setMinutes(const unsigned int recMinutes){
	minutes = recMinutes;
}

void timeData::setSeconds(const unsigned int recSeconds){
	seconds = recSeconds;
}

//<<<------------------------------------------------------------------------------------------>>>

dateData::dateData(const unsigned int weekDay, const unsigned int monthDay, const unsigned int month, const unsigned int year):
	weekDay(weekDay),
	monthDay(monthDay),
	month(month),
	year(year)
{}

void dateData::setWeekDay(const unsigned int recWeekDay){
	weekDay = recWeekDay;
}

void dateData::setMonthDay(const unsigned int recMonthDay){
	monthDay = recMonthDay;
}

void dateData::setMonth(const unsigned int recMonth){
	month = recMonth;
}

void dateData::setYear(const unsigned int recYear){
	year = recYear;
}



//<<<------------------------------------------------------------------------------------------->>>


DS3231::DS3231(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address):
	bus(bus),
	address(address),
	time(timeData()),
	date(dateData())
{}

void DS3231::getStatus(){
	bus.write(address).write(0x00);
	auto transaction = bus.read(address);
	transaction.read(status, 7);
	hwlib::wait_ms(30);

	hwlib::cout << ((status[2] & 0x0F) + ((status[2] >> 4) * 10) + (status[2] >> 5) * 20) << ":" << ((status[1] & 0x0F) + ((status[1] >> 4) & 0x07)) << hwlib::endl;
	time.setSeconds(status[0] & 0x0F);
	time.setMinutes((status[1] & 0x0F) + ((status[1] >> 4) & 0x07));
	time.setHours((status[2] & 0x0F) + ((status[2] >> 4) * 10) + (status[2] >> 5) * 20);

	date.setWeekDay(status[3] & 0x07);
	date.setMonthDay(status[4] & 0x0F);
	date.setMonth(status[5] & 0x0F);
	date.setYear(status[6] & 0x0F);
}