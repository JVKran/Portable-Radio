#include "hwlib.hpp"
#include "DS3231.hpp"

hwlib::ostream & operator<<(hwlib::ostream & lhs, const timeData & rhs){
  lhs << rhs.getHours() << ":" << rhs.getMinutes() << ":" << rhs.getSeconds();
  return lhs;
}

hwlib::ostream & operator<<(hwlib::ostream & lhs, const dateData & rhs){
  lhs << rhs.getWeekDay() << ", " << rhs.getMonthDay() << "-" << rhs.getMonth() << "-" << rhs.getYear();
  return lhs;
}

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
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Too Large Setting Handling: " << ((time.getHours() == 11) && time.getMinutes() == 40 && time.getSeconds() == 55) << hwlib::endl;

  time.setTime(10, 30);
  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Addition: " << time << " + " << lastTime << " = " << (time + lastTime) <<  hwlib::boolalpha << " " << ((time + lastTime) == timeData(16, 0, 0)) <<  hwlib::endl;
  lastTime.setHours(18);
  lastTime.setMinutes(15);
  lastTime.setSeconds(55);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Addition: " << time << " + " << lastTime << " = " << (time + lastTime) <<  hwlib::boolalpha << " " << ((time + lastTime) == timeData(4, 45, 55)) <<  hwlib::endl;

  lastTime.setTime(5, 30);
  time.setTime(10, 30);
  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Substraction: " << time << " - " << lastTime << " = " << (time - lastTime) << hwlib::boolalpha << "  " << ((time - lastTime) == timeData(5, 0, 0)) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Substraction: " << lastTime << " - " << time << " = " << (lastTime - time) << hwlib::boolalpha << " " << ((lastTime - time) == timeData(19, 0, 0)) <<  hwlib::endl;

  lastTime.setTime(5, 30);
  time.setTime(10, 30);
  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Equality: " << time << " == " << lastTime << " = " << (time == lastTime) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Unequality: " << lastTime << " != " << time << " = " << (lastTime != time) <<  hwlib::endl << hwlib::endl;

  hwlib::cout << "---------------------------------DATE------------------------------" << hwlib::endl << hwlib::endl;

  auto date = dateData(7, 30, 6, 2019);
  auto lastDate = dateData(10, 35, 14, 2019);

  hwlib::cout << hwlib::left << hwlib::setw(45) << "Initialization: " << (date.getWeekDay() == 7 && date.getMonthDay() == 30 && date.getMonth() == 6 && date.getYear() == 2019) << hwlib::endl;

  hwlib::cout << hwlib::left << hwlib::setw(45) << "Too Large Initialization Handling: " << (lastDate.getWeekDay() == 7 && lastDate.getMonthDay() == 31 && lastDate.getMonth() == 12 && lastDate.getYear() == 2019) << hwlib::endl;

  lastDate.setDate(7, 30, 6, 2019);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Setting: " << (lastDate.getWeekDay() == 7 && lastDate.getMonthDay() == 30 && lastDate.getMonth() == 6 && lastDate.getYear() == 2019) << hwlib::endl;

  lastDate.setDate(80, 90, 100, 2019);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Too Large Setting Handling: " << (lastDate.getWeekDay() == 7 && lastDate.getMonthDay() == 30 && lastDate.getMonth() == 6 && lastDate.getYear() == 2019) << hwlib::endl;

  date.setDate(1, 1, 1, 2019);
  lastDate.setDate(7, 30, 6, 0);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Addition: " << date << " + " << lastDate << " = " << (date + lastDate) <<  hwlib::boolalpha << " " << ((date + lastDate) == dateData(1, 31, 7, 2019)) <<  hwlib::endl;
  lastDate.setWeekDay(5);
  lastDate.setMonthDay(15);
  lastDate.setMonth(8);
  lastDate.setYear(2000);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Addition: " << date << " + " << lastDate << " = " << (date + lastDate) <<  hwlib::boolalpha << " " << ((date + lastDate) == dateData(6, 16, 9, 4019)) <<  hwlib::endl;

/*
  lastTime.setTime(5, 30);
  time.setTime(10, 30);
  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Substraction: " << time << " - " << lastTime << " = " << (time - lastTime) << hwlib::boolalpha << "  " << ((time - lastTime) == timeData(5, 0, 0)) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Substraction: " << lastTime << " - " << time << " = " << (lastTime - time) << hwlib::boolalpha << " " << ((lastTime - time) == timeData(19, 0, 0)) <<  hwlib::endl;

  lastTime.setTime(5, 30);
  time.setTime(10, 30);
  hwlib::cout << hwlib::endl << hwlib::left << hwlib::setw(45) << "Equality: " << time << " == " << lastTime << " = " << (time == lastTime) <<  hwlib::endl;
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Unequality: " << lastTime << " != " << time << " = " << (lastTime != time) <<  hwlib::endl << hwlib::endl;
*/

  hwlib::cout << hwlib::endl;
  for(unsigned int i = 0; i < 3; i++){
    time = clock.getTime();
    date = clock.getDate();
    hwlib::cout << "Time: " << time.getHours() << ":" << time.getMinutes() << ":" << time.getSeconds() << hwlib::endl;
    hwlib::cout << "Temperature: " << clock.getTemperature() << hwlib::endl;
    hwlib::cout << "Date: " << date.getMonthDay() << "-" << date.getMonth() << "-" << date.getYear() << hwlib::endl << hwlib::endl;
    hwlib::wait_ms(3000);
  }
  hwlib::cout << hwlib::endl;
  

}
