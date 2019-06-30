#include "hwlib.hpp"
#include "DS3231.hpp"

hwlib::ostream & operator<<(hwlib::ostream & lhs, const timeData & rhs){
  lhs << rhs.getHours() << ":" << rhs.getMinutes() << ":" << rhs.getSeconds();
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
  auto date = dateData(7, 30, 6, 2019);

  hwlib::wait_ms(1000);

  hwlib::cout << hwlib::left << hwlib::setw(45) << "Initialization: " << ((time.getHours() == 15) && time.getMinutes() == 20 && time.getSeconds() == 10) << hwlib::endl;

  hwlib::cout << hwlib::left << hwlib::setw(45) << "Too Large Initialization Handling: " << ((bigTime.getHours() == 0) && bigTime.getMinutes() == 0 && bigTime.getSeconds() == 0) << hwlib::endl;

  time.setTime(11, 40, 55);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Setting: " << ((time.getHours() == 11) && time.getMinutes() == 40 && time.getSeconds() == 55) << hwlib::endl;

  time.setTime(80, 90, 100);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Too Large Setting Handling: " << ((time.getHours() == 0) && time.getMinutes() == 0 && time.getSeconds() == 0) << hwlib::endl;

  time.setTime(10, 30);
  hwlib::cout << hwlib::left << hwlib::setw(45) << "Addition: " << time << " + " << lastTime << " = " << (time + lastTime) << ((time + lastTime) == timeData(16, 0, 0)) <<  hwlib::endl;


  for(;;){
    time = clock.getTime();
    date = clock.getDate();
    hwlib::cout << "Time: " << time.getHours() << ":" << time.getMinutes() << ":" << time.getSeconds() << hwlib::endl;
    hwlib::cout << "Temperature: " << clock.getTemperature() << hwlib::endl;
    hwlib::cout << "Date: " << date.getMonthDay() << "-" << date.getMonth() << "-" << date.getYear() << hwlib::endl << hwlib::endl;
    hwlib::wait_ms(30000);
  }
  

}
