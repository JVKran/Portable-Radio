#include "hwlib.hpp"
#include "DS3231.hpp"


int main( void ){
  namespace target = hwlib::target;

  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto clock = DS3231(i2c_bus);

  timeData time;
  dateData date;
  for(;;){
    time = clock.getTime();
    date = clock.getDate();
    hwlib::cout << "Time: " << time.getHours() << ":" << time.getMinutes() << ":" << time.getSeconds() << hwlib::endl;
    hwlib::cout << "Temperature: " << clock.getTemperature() << hwlib::endl;
    hwlib::cout << "Date: " << date.getMonthDay() << "-" << date.getMonth() << "-" << date.getYear() << hwlib::endl << hwlib::endl;
    hwlib::wait_ms(30000);
  }
  

}
