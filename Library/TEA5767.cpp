#include "hwlib.hpp"
#include "TEA5767.hpp"

TEA5767::TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address):
	bus(bus),
	address(address)
{}

void TEA5767::setFrequency(float frequency){
	//High side injection
	int pllFrequency = (float(4) * (frequency + 0.255) / 0.032768);
	//Low side injection
	//uint8_t pllFrequency = (4 * (frequency - 0.255) / 32.768)
	data[0] = pllFrequency >> 8;
	data[1] = pllFrequency & 0xFF;
	hwlib::cout << "data[0] = " << int(data[0]) << hwlib::endl;
	hwlib::cout << "data[1] = " << int(data[1]) << hwlib::endl;
	bus.write(address).write(data, 5);
}



