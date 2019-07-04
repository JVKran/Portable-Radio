#include "hwlib.hpp"
#include "DS3231.hpp"


int main( void ){
  namespace target = hwlib::target;

  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto clock = DS3231(i2c_bus);

  //Uncomment if time is allowed to get overwritten.
  //clock.setTime(timeData(9, 45));   //Through timeData object
  //clock.setDate(4, 4, 7, 2019);     //Or just by passing values

  auto curTime = clock.getTime();

  
  for(unsigned int i = 0; i < 3; i++){
    hwlib::cout << "Time: " << clock.getTime() << hwlib::endl;
    hwlib::cout << "Temperature: " << int(clock.getTemperature() * 10) << hwlib::endl;
    hwlib::cout << "Date: " << clock.getDate() << hwlib::endl << hwlib::endl;
    
    curTime = clock.getTime();
    curTime.setSeconds(curTime.getSeconds() + 10);

    hwlib::cout << "Time to Trigger: " << curTime << hwlib::endl;

    clock.clearAlarm(1);
    clock.changeFirstAlarm(curTime, dateData(0, 0, 1, 2019));   //Set moment when alarm should trigger
    clock.setFirstAlarm(14);                                    //Set what values have to match; specified in documentation.
    hwlib::cout << "Alarm set, should go in 10 seconds: ";

    hwlib::wait_ms(30);

    while(clock.checkAlarms() == 0){
      hwlib::wait_ms(1000);
      hwlib::cout << clock.getTime() << hwlib::endl;
    }

    hwlib::cout << "Triggered!" << hwlib::endl;
  }
}
