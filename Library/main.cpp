#include "hwlib.hpp"
#include "RDA5807.hpp"

int main( void ){
  namespace target = hwlib::target; 
    
  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto radio = RDA5807(i2c_bus);
  radio.begin();
  radio.setFrequency(101.2);    //100.1, 100.7
  hwlib::wait_ms(2000);
  for(;;){
    hwlib::wait_ms(50);
    radio.radioData.update();
    hwlib::cout << hwlib::left << hwlib::setw(30) << "Station Name: " << radio.radioData.stationName() << hwlib::endl;
    hwlib::wait_ms(50);
  }

}


//http://www.g.laroche.free.fr/english/rds/groupes/4/groupe4A.htm