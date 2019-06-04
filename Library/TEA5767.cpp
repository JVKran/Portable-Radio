#include "hwlib.hpp"
#include "TEA5767.hpp"

TEA5767::TEA5767(hwlib::i2c_bus_bit_banged_scl_sda & bus, uint8_t address):
	bus(bus),
	address(address)
{}

void TEA5767::test(){
	uint8_t data[5] = {0x30,0x1F,0x10,0x10,0xC0};
	uint8_t address = 0x60;
	bus.write(address, data, 5);
}


