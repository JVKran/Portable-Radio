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
  hwlib::cout << "Tuning to 100.7FM (Q-Music): " << hwlib::boolalpha << radio.isTuned() << int(radio.getFrequency()) << hwlib::endl;
  for(;;){
    hwlib::cout << hwlib::boolalpha << radio.isTuned() << hwlib::endl;
    hwlib::wait_ms(3000);
  }
}
