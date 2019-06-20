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
	radio.audioSettings(false, true, true);
	hwlib::cout << "Tuning to 100.7" << hwlib::endl;
	radio.setFrequency(88);
	hwlib::wait_ms(2000);
	for(;;){
		radio.altSearch(1, 2, 0.2);
		hwlib::cout << int(radio.getFrequency()) << hwlib::endl;
		hwlib::wait_ms(2000);
	}

}
