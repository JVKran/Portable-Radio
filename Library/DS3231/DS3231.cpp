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

void DS3231::setData(){
	auto transaction = bus.write(address);
	transaction.write(0x00);
	transaction.write(data, 8);
}

void DS3231::getStatus(){
	bus.write(address).write(0x00);
	auto transaction = bus.read(address);
	transaction.read(status, 7);
	hwlib::wait_ms(30);

	hwlib::cout << ((status[2] & 0x0F) + ((status[2] >> 4) * 10) + (status[2] >> 5) * 20) << ":" << ((status[1] & 0x0F) + ((status[1] >> 4) & 0x07) * 10) << ":" << ((status[0] & 0x0F) + ((status[0] >> 4) & 0x07) * 10) << hwlib::endl;
	time.setSeconds((status[0] & 0x0F) + ((status[0] >> 4) & 0x07) * 10);
	time.setMinutes((status[1] & 0x0F) + ((status[1] >> 4) & 0x07));
	time.setHours((status[2] & 0x0F) + ((status[2] >> 4) * 10) + (status[2] >> 5) * 20);

	date.setWeekDay(status[3] & 0x07);
	date.setMonthDay((status[4] & 0x0F) + (((status[4] >> 4) & 0x03) * 10));
	date.setMonth(((status[5] & 0x0F) + ((status[5] >> 4) & 0x01) * 10));
	date.setYear(((status[6] & 0x0F) + (((status[6] >> 4) & 0x0F) * 10) + (2000 - (100 * (status[5] >> 7 & 1)))));
	hwlib::cout << (status[4] & 0x0F) + (((status[4] >> 4) & 0x03) * 10) << "-" << ((status[5] & 0x0F) + ((status[5] >> 4) & 0x01) * 10) << "-" <<  ((status[6] & 0x0F) + (((status[6] >> 4) & 0x0F) * 10) + (2000 - (100 * (status[5] >> 7 & 1)))) << hwlib::endl;
}

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