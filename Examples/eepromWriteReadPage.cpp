#include "hwlib.hpp"
#include "A24C256.hpp"

int main( void ){
    namespace target = hwlib::target; 

    auto scl = target::pin_oc( target::pins::d8 );
    auto sda = target::pin_oc( target::pins::d9 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

	auto memory = A24C256(i2c_bus);

	char data[]={"At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et expedita distinctio."};
	memory.write(0, data);

	uint8_t receivedData[353];
	memory.read(0, 353, receivedData);
	for(unsigned int i = 0; i < 353; i++){
		hwlib::cout << char(receivedData[i]);
	}
}

