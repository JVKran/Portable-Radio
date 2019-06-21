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
  radio.init();
  hwlib::wait_ms(5000);
  radio.setMute(true);
  hwlib::wait_ms(5000);
  radio.setMute(false);
}
