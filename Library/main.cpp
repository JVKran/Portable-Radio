#include "hwlib.hpp"
#include "RDA5807.hpp"

int main( void ){
    namespace target = hwlib::target; 
    
    auto scl = target::pin_oc( target::pins::d11 );
    auto sda = target::pin_oc( target::pins::d12 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

	auto radio = RDA5807(i2c_bus);
	radio.setFrequency(1007);

}
