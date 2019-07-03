/// @file

#include "hwlib.hpp"
#include "DS3231.hpp"

/// \brief
/// Test
/// \details
/// This program tests ALL functionality of the DS3231 Real Time clock.
int main( void ){
  namespace target = hwlib::target;

  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto clock = DS3231(i2c_bus);

  auto time = timeData(15, 20, 10);
  auto lastTime = timeData(5, 30);
  auto bigTime = timeData(50, 60, 80);

  hwlib::wait_ms(1000);

  hwlib::cout << "---------------------------------TIME------------------------------" << hwlib::endl << hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::boolalpha << hwlib::setw(45) << "Initialization: " << ((time.getHours() == 15) && time.getMinutes() == 20 && time.getSeconds() == 10) << hwlib::endl;

  hwlib::cout << hwlib::left << hwlib::setw(45) << "Too Large Initialization Handling: " << ((bigTime.getHours() == 0) && bigTime.getMinutes() == 0 && bigTime.getSeconds() == 0) << hwlib::endl;

  time.setTime(11, 40, 55);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Setting: " << ((time.getHours() == 11) && time.getMinutes() == 40 && time.getSeconds() == 55) << hwlib::endl;

  time.setTime(80, 90, 100);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Too Large Setting Handling: " << (time == timeData(0, 0, 0)) << hwlib::endl;

  time.setTime(10, 30);
  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Addition: " << time << " + " << lastTime << " = " << (time + lastTime) <<  hwlib::boolalpha << " " << ((time + lastTime) == timeData(16, 0, 0)) <<  hwlib::endl;
  lastTime.setHours(18);
  lastTime.setMinutes(15);
  lastTime.setSeconds(55);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Addition: " << time << " + " << lastTime << " = " << (time + lastTime) <<  hwlib::boolalpha << " " << ((time + lastTime) == timeData(4, 45, 55)) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Addition: " << timeData(0, 0) << " + " << timeData(0, 0) << " = " << (timeData(0+0, 0+0)) << " true" << hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Addition: " << timeData(23, 59) << " + " << timeData(23, 59) << " = " << (timeData(23, 59) + timeData(23, 59)) << " true" <<  hwlib::endl;

  lastTime.setTime(5, 30);
  time.setTime(10, 30);
  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Substraction: " << time << " - " << lastTime << " = " << (time - lastTime) << hwlib::boolalpha << "  " << ((time - lastTime) == timeData(5, 0, 0)) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Substraction: " << lastTime << " - " << time << " = " << (lastTime - time) << hwlib::boolalpha << " " << ((lastTime - time) == timeData(19, 0, 0)) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Substraction: " << timeData(23, 59) << " - " << timeData(23, 59) << " = " << (timeData(23, 59) - timeData(23, 59)) << " true" << hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Substraction: " << timeData(0, 15) << " - " << timeData(0, 30) << " = " << (timeData(0, 15) - timeData(0, 30)) << " true" <<  hwlib::endl;

  lastTime.setTime(5, 30);
  time.setTime(10, 30);
  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Comparison: " << time << " < " << lastTime << " = " << (time < lastTime) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Comparison: " << timeData(11, 30) << " < " << timeData(11, 30, 10) << " = " << (timeData(11, 30) < timeData(11, 30, 10)) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Comparison: " << time << " <= " << lastTime << " = " << (time <= lastTime) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Comparison: " << time << " > " << lastTime << " = " << (time > lastTime) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Comparison: " << time << " >= " << lastTime << " = " << (time >= lastTime) <<  hwlib::endl;

  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Equality: " << time << " == " << lastTime << " = " << (time == lastTime) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Unequality: " << lastTime << " != " << time << " = " << (lastTime != time) <<  hwlib::endl << hwlib::endl;

  hwlib::cout << "---------------------------------DATE------------------------------" << hwlib::endl << hwlib::endl;

  auto date = dateData(7, 30, 6, 2019);
  auto lastDate = dateData(10, 35, 14, 2019);

  hwlib::cout << hwlib::left << hwlib::setw(45) << "Initialization: " << (date.getWeekDay() == 7 && date.getMonthDay() == 30 && date.getMonth() == 6 && date.getYear() == 2019) << hwlib::endl;

  hwlib::cout << hwlib::left << hwlib::setw(45) << "Too Large Initialization Handling: " << (lastDate.getWeekDay() == 1 && lastDate.getMonthDay() == 1 && lastDate.getMonth() == 1 && lastDate.getYear() == 2019) << hwlib::endl;

  lastDate.setDate(7, 30, 6, 2019);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Setting: " << (lastDate.getWeekDay() == 7 && lastDate.getMonthDay() == 30 && lastDate.getMonth() == 6 && lastDate.getYear() == 2019) << hwlib::endl;

  lastDate.setDate(80, 90, 100, 2019);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Too Large Setting Handling: " << (lastDate.getWeekDay() == 1 && lastDate.getMonthDay() == 1 && lastDate.getMonth() == 1 && lastDate.getYear() == 2019) << hwlib::endl;

  date.setDate(1, 1, 1, 2019);
  lastDate.setDate(7, 30, 6, 0);
  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Addition: " << date.getWeekDay() << ", " << date << " + " << lastDate.getWeekDay() << ", " << lastDate << " = " << (date + lastDate) <<  hwlib::boolalpha << " " << ((date + lastDate) == dateData(1, 1, 8, 2019)) <<  hwlib::endl;

  lastDate.setWeekDay(5);
  lastDate.setMonthDay(15);
  lastDate.setMonth(8);
  lastDate.setYear(2000);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Setting and Addition: " << date.getWeekDay() << ", " << date << " + " << lastDate.getWeekDay() << ", " << lastDate << " = " << (date + lastDate) <<  hwlib::boolalpha << " " << ((date + lastDate) == dateData(6, 16, 9, 4019)) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Addition: " << dateData(6, 20, 12, 2019) << " + " << dateData(6, 20, 5, 0) << " = " << (dateData(6, 20, 5, 0) +  dateData(6, 20, 12, 2019)) <<  hwlib::boolalpha << " true" <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Addition: " << dateData(1, 1, 1, 2000) << " + " << dateData(1, 1, 1, 0) << " = " << (dateData(1, 1, 1, 2000) +  dateData(1, 1, 1, 0)) <<  hwlib::boolalpha << " true" <<  hwlib::endl;

  date.setDate(5, 8, 10, 2019);
  lastDate.setDate(4, 30, 8, 2000);
  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Substraction: " << date.getWeekDay() << ", " << date << " - " << lastDate.getWeekDay() << ", " << lastDate << " = " << (date - lastDate) << hwlib::boolalpha << "  " << ((date - lastDate) == dateData(1, 8, 1, 19)) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Substraction: " << lastDate.getWeekDay() << ", " << lastDate << " - " << date.getWeekDay() << ", " << date << " = " << (lastDate - date) << hwlib::boolalpha << " " << ((lastDate - date) == dateData(6, 22, 10, 0)) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Substraction: " << dateData(6, 20, 12, 2019) << " - " << dateData(6, 20, 5, 0) << " = " << (dateData(6, 20, 12, 0) -  dateData(6, 20, 5, 2019)) <<  hwlib::boolalpha << " true" <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Substraction: " << dateData(6, 1, 1, 2019) << " - " << dateData(6, 6, 6, 0) << " = " << (dateData(6, 1, 1, 2019) -  dateData(6, 6, 6, 0)) <<  hwlib::boolalpha << " true" <<  hwlib::endl;

  lastTime.setTime(5, 30);
  time.setTime(10, 30);
  
  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Comparison: " << date << " < " << lastDate << " = " << (date < lastDate) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Comparison: " << date << " <= " << lastDate << " = " << (date <= lastDate) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Comparison: " << dateData(1, 1, 12, 2019) << " <= " << dateData(1, 1, 12, 2019) << " = " << (dateData(1, 1, 12, 2019) <= dateData(1, 1, 12, 2019)) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Comparison: " << date << " > " << lastDate << " = " << (date > lastDate) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Comparison: " << dateData(1, 1, 12, 2019) << " > " << dateData(1, 1, 12, 2019) << " = " << (dateData(1, 1, 12, 2019) > dateData(1, 1, 12, 2019)) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Comparison: " << date << " >= " << lastDate << " = " << (date >= lastDate) <<  hwlib::endl;

  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Equality: " << date.getWeekDay() << ", " << date << " == " << lastDate.getWeekDay() << ", " << lastDate << " = " << (date == lastDate) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Unequality: " << date.getWeekDay() << ", " << date << " != " << lastDate.getWeekDay() << ", " << lastDate << " = " << (date != lastDate) <<  hwlib::endl << hwlib::endl;


  hwlib::cout << "-------------------------------DS3231--------------------------------" << hwlib::endl << hwlib::endl;

  hwlib::cout << hwlib::left << hwlib::setw(45) << "Initialization: " << (clock.getAddress() == 0x68) << hwlib::endl;

  hwlib::cout << hwlib::endl;
  for(unsigned int i = 0; i < 3; i++){
    time = clock.getTime();
    date = clock.getDate();
    hwlib::cout << "Time: " << clock.getTime() << hwlib::endl;
    hwlib::cout << "Temperature: " << int(clock.getTemperature() * 10) << hwlib::endl;
    hwlib::cout << "Date: " << clock.getDate() << hwlib::endl << hwlib::endl;
    hwlib::wait_ms(3000);
  }
  hwlib::cout << hwlib::endl;
  
  //Uncomment if time is allowed to get lost. You'll have to set it again later.
  //hwlib::cout << hwlib::left << hwlib::setw(45) << "Set time to 0:0:0 : ";
  //clock.setTime(0, 0, 0);
  //hwlib::cout << (clock.getTime() == timeData(0, 0, 0)) << hwlib::endl;

  //The alarm other than the one you want to change and activate first has to be set; puts user more in control.
  clock.setSecondAlarm(16);

  auto curTime = clock.getTime();
  curTime.setSeconds(curTime.getSeconds() + 10);
  clock.changeFirstAlarm(curTime, dateData(0, 0, 1, 2019));
  clock.setFirstAlarm(14);
  hwlib::cout << "Alarm set, should go in 10 seconds: ";


  while(clock.checkAlarms() == 0){
    hwlib::wait_ms(200);
  }

  hwlib::cout << "Triggered!" << hwlib::endl;

}
