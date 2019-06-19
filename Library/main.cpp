#include "hwlib.hpp"
#include "TEA5767.hpp"
#include <stdio.h>

int main( void ){
    namespace target = hwlib::target; 
    
    auto scl = target::pin_oc( target::pins::d8 );
    auto sda = target::pin_oc( target::pins::d9 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

	auto radio = TEA5767(i2c_bus);
	radio.setStereo(true);
	radio.audioSettings(true, true, true);
	radio.setFrequency(100.7);
	for(;;){
		radio.search(1, 3);
		hwlib::cout << int(radio.getFrequency()) << hwlib::endl;
		hwlib::wait_ms(2000);
	}
}
