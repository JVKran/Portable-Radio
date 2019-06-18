#include "hwlib.hpp"
#include "A24C256.hpp"

int main( void ){
    namespace target = hwlib::target; 

    auto scl = target::pin_oc( target::pins::d8 );
    auto sda = target::pin_oc( target::pins::d9 );
	auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

	auto memory = A24C256(i2c_bus);
	auto largeMemory = A24C256(i2c_bus, 512);

	//Write one single byte at location 0 and retrieve it
	memory.write(0, 'c');
	hwlib::cout << char(memory.read(0)) << hwlib::endl;

	//Write 12 bytes to location 20
	char data[]={"Hello World!"};
	memory.write(20, data);

	//Retrieve multiple bytes (preffered)
	uint8_t receivedData[12];
	memory.read(20, 12, receivedData);
	for(unsigned int i = 0; i < 12; i++){
		hwlib::cout << char(receivedData[i]);
	}

	hwlib::cout << hwlib::endl;

	//Another way to retrieve multiple bytes
	for(unsigned int i = 20; i < 32; i++){
		hwlib::cout << char(memory.read(i));
	}

	hwlib::cout << hwlib::endl;

	memory.write(0, 100);
	hwlib::cout << memory.read(0) << hwlib::endl;

	memory.write(0, true);
	hwlib::cout << (bool(memory.read(0)) && 1);

}