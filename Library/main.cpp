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
  radio.setFrequency(100.7);
  hwlib::wait_ms(1000);
  for(;;){
    radio.updateRDS();
    radio.processRDS();
    hwlib::wait_ms(200);
  }

}
