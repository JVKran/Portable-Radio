#include "hwlib.hpp"
#include "A24C256.hpp"
#include "TEA5767.hpp"

int main( void ){
    namespace target = hwlib::target; 

    auto scl = target::pin_oc( target::pins::d8 );
    auto sda = target::pin_oc( target::pins::d9 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

	auto memory = A24C256(i2c_bus);

	char data[]={"Hello-1234567890-and-abcdefghijklmnopqrstuvwxyz-Goodbye\n"};
	memory.write(0, data);
	for(unsigned int i = 0; i < 120; i++){
		hwlib::cout << char(memory.readByte(i)) << hwlib::endl;
	}
}
