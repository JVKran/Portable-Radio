#include "hwlib.hpp"
#include "TEA5767.hpp"
#include "RDA5807.hpp"
#include "GUI.hpp"
#include "KY040.hpp"

int main( void ){
  namespace target = hwlib::target;

  auto scl = target::pin_oc( target::pins::d8 );
  auto sda = target::pin_oc( target::pins::d9 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

	auto radio = RDA5807(i2c_bus);
  radio.begin();
  radio.setFrequency(100.7);
  /*
  hwlib::wait_ms(2000);
  radio.setMute(true);
  hwlib::wait_ms(2000);
  radio.setMute(false);
  hwlib::cout << int(radio.getFrequency()) << hwlib::endl;
  hwlib::wait_ms(5000);
  radio.setFrequency(101.2);
  hwlib::wait_ms(2000);
  hwlib::cout << radio.signalStrength() << hwlib::endl;
  */
  hwlib::wait_ms(5000);
  for(;;){
      hwlib::cout << radio.signalStrength() << ", " << radio.isReady() << hwlib::endl;
      hwlib::wait_ms(3000);
      radio.processRDS();
  }
  hwlib::wait_ms(500);
}
