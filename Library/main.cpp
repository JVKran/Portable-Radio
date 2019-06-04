#include "hwlib.hpp"
#include "TEA5767.hpp"

int main( void ){
    namespace target = hwlib::target; 
    
    auto scl = target::pin_oc( target::pins::d8 );
    auto sda = target::pin_oc( target::pins::d9 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

	auto radio = TEA5767(i2c_bus);

	uint8_t data[5] = {0x30,0x1F,0x10,0x10,0xC0};
	uint8_t address = 0x60;
	i2c_bus.write(address, data, 5);

   
     	

}





         






         

