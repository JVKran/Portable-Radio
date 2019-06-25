#include "hwlib.hpp"
#include "RDA5807.hpp"

int main( void ){
  namespace target = hwlib::target; 
    
  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  auto radio = RDA5807(i2c_bus);
  radio.begin();
  hwlib::cout << "Tuning to 100.7FM (Q-Music): ";
  radio.setFrequency(101.2);
  hwlib::wait_ms(2000);
  for(;;){
    radio.seekChannel(1);
    hwlib::wait_ms(2000);
    for(unsigned int i = 0; i < 3; i++){
      hwlib::cout << radio.getStationName(2) << hwlib::endl;
      //radio.processRDS();
      hwlib::wait_ms(2000);
    }
  }

}


//http://www.g.laroche.free.fr/english/rds/groupes/4/groupe4A.htm